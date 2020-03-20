/*--------------------------------------------------------------------*/
/*--- CryptoSnooper: A sneak peek into crypto rotines.   cs_main.c ---*/
/*--------------------------------------------------------------------*/

#include "pub_tool_basics.h"
#include "pub_tool_tooliface.h"

static void cs_post_clo_init(void)
{
}

enum
{
NO_OPERATION,
LEFT_SHIFT,
RIGHT_SHIFT,
LEFT_ROTATE,
RIGHT_ROTATE,
XOR_RES,
XOR_SRC,
ADD_DONE
};

int sbCount;
int printThisSB;

int sha1RoundsDetected;
int valFailure;
int numLeftRotDone;

typedef struct
{
	int numAddsDone;
	int numLeftRotateDone;
	int numXorDone;
	int numXorSrc;
	int numConstVals;
	int sha1Rounds
}sha1Tracker;

typedef struct
{
	int operationState;
	int shiftAmt;
	sha1Tracker shaObject;
}instrTracker;

typedef struct
{
	int regNum;
	instrTracker state;
}tempTracker;

typedef struct
{
	int lastUpdTempNum;
	instrTracker state;
}regTracker;

regTracker regStates[8];
tempTracker tempStates[500];

regTracker bkupRegStates[8];

instrTracker handleExpr(IRExpr *);

//temporary method till handling all registers
int findRegIndex(int offset)
{
	int res = -1;
	if(offset <= 36)
		res = offset/4 - 2;
	return res;
}

static
IRSB* cs_instrument ( VgCallbackClosure* closure,
                      IRSB* bb,
                      const VexGuestLayout* layout, 
                      const VexGuestExtents* vge,
                      const VexArchInfo* archinfo_host,
                      IRType gWordTy, IRType hWordTy )
{
	int ti;
	for(ti=0;ti<500;ti++)
	{
		tempStates[ti].regNum = -1;
        	tempStates[ti].state.operationState = NO_OPERATION;
        	tempStates[ti].state.shiftAmt = 0;
        	tempStates[ti].state.shaObject.numAddsDone = 0;
        	tempStates[ti].state.shaObject.numLeftRotateDone = 0;
        	tempStates[ti].state.shaObject.numXorDone = 0;
        	tempStates[ti].state.shaObject.numXorSrc = 0;
        	tempStates[ti].state.shaObject.numConstVals = 0;
        	tempStates[ti].state.shaObject.sha1Rounds = 0;
    	}
    	
    	for(ti=0;ti<8;ti++)
    	{
    		regStates[ti].lastUpdTempNum = -1;
    		bkupRegStates[ti] = regStates[ti];
    	}
	
	int i=0;
	while(i<bb->stmts_used)
	{
		IRStmt *stmt = bb->stmts[i];
		int temp;
		int regIndex;
		IRExpr *expr;
		switch(stmt->tag)
		{
			case Ist_Put:
				expr = stmt->Ist.Put.data;
				regIndex = findRegIndex(stmt->Ist.Put.offset);
				if(regIndex != -1)
				{
					instrTracker exprRes = handleExpr(expr);
					regStates[regIndex].state = exprRes;
					if(expr->tag == Iex_RdTmp)
					{
						int tmpID = expr->Iex.RdTmp.tmp;
						tmpID = tmpID % 500;
						tempStates[tmpID].regNum = regIndex;
						regStates[regIndex].lastUpdTempNum = tmpID;
					}
				}
				break;
			case Ist_WrTmp:
				temp = stmt->Ist.WrTmp.tmp;
				temp = temp % 500;
				expr = stmt->Ist.WrTmp.data;
				tempStates[temp].state = handleExpr(expr);
				if(expr->tag == Iex_Get)
					tempStates[temp].regNum = findRegIndex(expr->Iex.Get.offset);
				break;
			case Ist_Store:
				//handled in future
				break;
		}
		i++;
	}
	
	if(printThisSB == 1)
	{
		int instrCount = 0;
		VG_(printf)("super block #%d==================================================================\n",sbCount);
		
		for(i=0;i<8;i++)
		{
			VG_(printf)("%d %d\n",i,bkupRegStates[i]);
		}
		
		i = 0;
		while(i<bb->stmts_used)
		{
			ppIRStmt(bb->stmts[i]);
			VG_(printf)("\n");
			if(bb->stmts[i]->tag == Ist_IMark)
				instrCount++;
			i++;
		}
		
		//VG_(printf)("super block #%d has %d instructions\n",sbCount,instrCount);
		
		printThisSB = 0;
	}
	
	sbCount++;

	return bb;
}

UChar getConstantValFromExpr(IRExpr *constExpr)
{
	int res = 0;
	if(constExpr->tag == Iex_Const)
	{
		IRConst *c = constExpr->Iex.Const.con;
		switch(c->tag)
		{
			case Ico_U8:
				res = c->Ico.U8;
				break;
			case Ico_U32:
				res = c->Ico.U32;
		}
	}
	return res;
}

instrTracker detectRotation(IRExpr *a1,IRExpr *a2)
{
	instrTracker state1 = handleExpr(a1);
	instrTracker state2 = handleExpr(a2);
	
	instrTracker res;
	res.operationState = NO_OPERATION;
	res.shiftAmt = 0;
	res.shaObject.numAddsDone = 0;
	res.shaObject.numLeftRotateDone = 0;
	res.shaObject.numXorDone = 0;
	res.shaObject.numXorSrc = 0;
	res.shaObject.numConstVals = 0;
	res.shaObject.sha1Rounds = 0;

	if((state1.operationState == LEFT_SHIFT && state2.operationState == RIGHT_SHIFT) || (state1.operationState == RIGHT_SHIFT && state2.operationState == LEFT_SHIFT))
	{
		if(state1.shiftAmt + state2.shiftAmt == 32)
		{
			res.operationState = LEFT_ROTATE;
			res.shiftAmt  = (state1.operationState == LEFT_SHIFT) ? state1.shiftAmt : state2.shiftAmt;
			res.shaObject.sha1Rounds = state1.shaObject.sha1Rounds;
			if(res.shiftAmt == 5)
			{
				numLeftRotDone++;
				VG_(printf)("rol5 done %d time(s) round is %d sb#%d\n",numLeftRotDone,sha1RoundsDetected,sbCount);
				//if(numLeftRotDone == 45)
					//printThisSB = 1;
				valFailure = 1;
			}
		}
	}
	
	return res;
}

int isThisSha1Round1(sha1Tracker sha1Object)
{
	if(sha1Object.numAddsDone == 3 && sha1Object.numLeftRotateDone == 1 && sha1Object.numXorDone == 1 && sha1Object.numConstVals == 2)
		return 1;
	return 0;
}

int isThisSha1RoundBetween2And80(sha1Tracker sha1Object)
{
	if(sha1Object.numAddsDone == 3 && sha1Object.numLeftRotateDone == 1 && sha1Object.numXorSrc == 1 && sha1Object.numXorDone == 1 && sha1Object.numConstVals == 1)
		return 1;

	return 0;
}

sha1Tracker shaObjectValidator(sha1Tracker sha1Object)
{
	sha1Tracker res;
	res.numAddsDone = 0;
	res.numLeftRotateDone = 0;
	res.numXorDone = 0;
	res.numXorSrc = 0;
	res.numConstVals = 0;
	res.sha1Rounds = 0;

	if(sha1Object.numAddsDone <= 3 && sha1Object.numLeftRotateDone <= 1 && sha1Object.numXorSrc <= 1 && sha1Object.numXorDone <= 1 && sha1Object.numConstVals <= 2 && sha1Object.sha1Rounds <= 80)
		return sha1Object;
	if(valFailure == 1)
		//printThisSB = 1;
		VG_(printf)("validation failed %d %d %d %d %d sb#%d\n",sha1Object.numAddsDone,sha1Object.numLeftRotateDone,sha1Object.numXorSrc,sha1Object.numXorDone,sha1Object.numConstVals,sbCount);
	return res;
}

sha1Tracker shaObjectCombiner(sha1Tracker sObj1,sha1Tracker sObj2)
{
	sha1Tracker res;
	res.numAddsDone = 0;
	res.numLeftRotateDone = 0;
	res.numXorDone = 0;
	res.numXorSrc = 0;
	res.numConstVals = 0;
	res.sha1Rounds = 0;

	res.numAddsDone       = sObj1.numAddsDone          + sObj2.numAddsDone + 1;
	res.numLeftRotateDone = sObj1.numLeftRotateDone    + sObj2.numLeftRotateDone;
	res.numXorSrc	      = sObj1.numXorSrc            + sObj2.numXorSrc;
	res.numXorDone        = sObj1.numXorDone           + sObj2.numXorDone;
	res.numConstVals      = sObj1.numConstVals         + sObj2.numConstVals;

	res.sha1Rounds        = sObj1.sha1Rounds           + sObj2.sha1Rounds;

	return res;
}

sha1Tracker shaObjectBuilder(IRExpr *arg)
{
	sha1Tracker res;
	res.numAddsDone = 0;
	res.numLeftRotateDone = 0;
	res.numXorSrc = 0;
	res.numXorDone = 0;
	res.numConstVals = 0;
	res.sha1Rounds = 0;

	instrTracker a;

	if(arg->tag != Iex_Const)
	{
		a = handleExpr(arg);
		res.numAddsDone = a.shaObject.numAddsDone;
		res.numLeftRotateDone = a.shaObject.numLeftRotateDone;
		res.numXorSrc = a.shaObject.numXorSrc;
		res.numXorDone = a.shaObject.numXorDone;
		res.numConstVals = a.shaObject.numConstVals;
		switch(a.operationState)
		{
			case LEFT_ROTATE:
				res.numLeftRotateDone++;
				res.sha1Rounds = a.shaObject.sha1Rounds;
				break;
			case XOR_SRC:
				res.numXorSrc++;
				break;
			case XOR_RES:
				res.numXorDone++;
				break;
			case NO_OPERATION:
				res.numConstVals++;
				break;
        	}
	}
	return res;
}

instrTracker handleAddArguments(IRExpr *arg1,IRExpr *arg2)
{
	instrTracker res;
	res.operationState = NO_OPERATION;
	res.shiftAmt = 0;
	res.shaObject.numAddsDone = 0;
	res.shaObject.numLeftRotateDone = 0;
	res.shaObject.numXorSrc = 0;
	res.shaObject.numXorDone = 0;
	res.shaObject.numConstVals = 0;
	res.shaObject.sha1Rounds = 0;

	sha1Tracker shaObj1,shaObj2;

	shaObj1 = shaObjectBuilder(arg1);
	shaObj2 = shaObjectBuilder(arg2);

	res.shaObject = shaObjectCombiner(shaObj1,shaObj2);

	res.shaObject = shaObjectValidator(res.shaObject);

	return res;
}

void handleXorSrcArgument(IRExpr *argument)
{
	int tmp,regMappedTmp;
	if(argument->tag == Iex_RdTmp)
	{
		tmp = argument->Iex.RdTmp.tmp;
		tmp = tmp % 500;
		tempStates[tmp].state.operationState = XOR_SRC;
		regMappedTmp = tempStates[tmp].regNum;
		if(regMappedTmp != -1)
		{
			if(regStates[regMappedTmp].lastUpdTempNum == -1 || regStates[regMappedTmp].lastUpdTempNum == tmp)
				regStates[regMappedTmp].state.operationState = XOR_SRC;
		}
	}
}

instrTracker handleBinOpExpr(IRExpr *expr)
{
	instrTracker res;
	res.operationState = NO_OPERATION;
	res.shiftAmt  = 0;
	res.shaObject.numAddsDone = 0;
	res.shaObject.numLeftRotateDone = 0;
	res.shaObject.numXorSrc = 0;
	res.shaObject.numXorDone = 0;
	res.shaObject.numConstVals = 0;
	res.shaObject.sha1Rounds = 0;

	IRExpr *argument1,*argument2;

	int opCode = expr->Iex.Binop.op;

	argument1 = expr->Iex.Binop.arg1;
	argument2 = expr->Iex.Binop.arg2;

	switch(opCode)
	{
		case Iop_Shr32:
			res.operationState = RIGHT_SHIFT;
			res.shiftAmt  = getConstantValFromExpr(argument2);
			res.shaObject.sha1Rounds = handleExpr(argument1).shaObject.sha1Rounds;
			break;
		case Iop_Shl32:
			res.operationState = LEFT_SHIFT;
			res.shiftAmt  = getConstantValFromExpr(argument2);
			res.shaObject.sha1Rounds = handleExpr(argument1).shaObject.sha1Rounds;
			break;
		case Iop_Or32:
			res = detectRotation(argument1,argument2);
			break;
		case Iop_Xor32:
			handleXorSrcArgument(argument1);
			handleXorSrcArgument(argument2);
			res.operationState = XOR_RES;
			break;
		case Iop_Add32:
			res = handleAddArguments(argument1,argument2);
			res.operationState = ADD_DONE;
			if(res.shaObject.numLeftRotateDone == 1)
				VG_(printf)("%d %d %d %d %d sb#%d\n",res.shaObject.numAddsDone,res.shaObject.numLeftRotateDone,res.shaObject.numXorSrc,res.shaObject.numXorDone,res.shaObject.numConstVals,sbCount);
			if((res.shaObject.sha1Rounds == 0 && isThisSha1Round1(res.shaObject) == 1) || isThisSha1RoundBetween2And80(res.shaObject) == 1)
			{
				sha1RoundsDetected++;
				res.shaObject.sha1Rounds++;
				res.operationState = NO_OPERATION;
				res.shaObject.numAddsDone = 0;
				res.shaObject.numLeftRotateDone = 0;
				res.shaObject.numXorSrc = 0;
				res.shaObject.numXorDone = 0;
				res.shaObject.numConstVals = 0;
			}
			if(numLeftRotDone == 80)
				valFailure = 0;
			break;
	}

	return res;
}

instrTracker handleExpr(IRExpr *expr)
{
	instrTracker res;
	res.operationState = NO_OPERATION;
	res.shiftAmt  = 0;
	res.shaObject.numAddsDone = 0;
	res.shaObject.numLeftRotateDone = 0;
	res.shaObject.numXorSrc = 0;
	res.shaObject.numXorDone = 0;
	res.shaObject.numConstVals = 0;
	res.shaObject.sha1Rounds = 0;

	int regNum,tempNum;

	switch(expr->tag)
	{
		case Iex_Get:
			regNum = findRegIndex(expr->Iex.Get.offset);
			if(regNum != -1)
				res = regStates[regNum].state;
			break;
		case Iex_RdTmp:
			tempNum = expr->Iex.RdTmp.tmp;
			tempNum = tempNum % 500;
			res = tempStates[tempNum].state;
			break;
		case Iex_Binop:
			res = handleBinOpExpr(expr);
			break;
		case Iex_CCall:
			break;
	}

	return res;
}

static void cs_fini(Int exitcode)
{
	VG_(printf)("number of sha1 rounds found:%d\n",sha1RoundsDetected);
}

static void cs_pre_clo_init(void)
{
	VG_(details_name)            ("CryptoSnooper");
	VG_(details_version)         (NULL);
	VG_(details_description)     ("Snoop crypto details of the child");
	VG_(details_copyright_author)("author:Krishnan");
	VG_(details_bug_reports_to)  (VG_BUGS_TO);

	VG_(details_avg_translation_sizeB) ( 275 );

	VG_(basic_tool_funcs)        (cs_post_clo_init,
                                 cs_instrument,
                                 cs_fini);

	int i;
	for(i=0;i<8;i++)
   	{
   		regStates[i].lastUpdTempNum = -1;
       		regStates[i].state.operationState = NO_OPERATION;
	       	regStates[i].state.shiftAmt  = 0;
       		regStates[i].state.shaObject.numAddsDone = 0;
       		regStates[i].state.shaObject.numLeftRotateDone = 0;
       		regStates[i].state.shaObject.numXorDone = 0;
       		regStates[i].state.shaObject.numConstVals = 0;
   	}

	sha1RoundsDetected = 0;
	numLeftRotDone = 0;
	valFailure = 0;
	
	sbCount = 1;
	printThisSB = 0;
   	/* No needs, no core events to track */
}

VG_DETERMINE_INTERFACE_VERSION(cs_pre_clo_init)

/*--------------------------------------------------------------------*/
/*--- end                                                          ---*/
/*--------------------------------------------------------------------*/
