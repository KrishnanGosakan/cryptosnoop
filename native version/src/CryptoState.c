#include "../header/ShadowRegister.h"
#include "../header/InstructionOperandHandler.h"

#include <string.h>

int sha256_roundCount = 0;
int sha1_roundCount = 0;

int ror5=0,ror9=0,ror25=0,ror30=0;
int rol5=0,rol9=0,rol25=0,rol30=0;

int CheckState(ud_t instruction,int childPID,const char *instr)
{
	if(instruction.mnemonic == UD_Iror)
	{
		if(getOperandValue(instruction.operand[1],childPID) == 5)
		{
			ror5++;
			printf("ror5 done %d time(s)\n",ror5);
		}
		else if(getOperandValue(instruction.operand[1],childPID) == 9)
		{
			ror9++;
			printf("ror9 done %d time(s)\n",ror9);
		}
		else if(getOperandValue(instruction.operand[1],childPID) == 25)
		{
			ror25++;
			printf("ror25 done %d time(s)\n",ror25);
		}
		else if(getOperandValue(instruction.operand[1],childPID) == 30)
		{
			ror30++;
			printf("ror30 done %d time(s)\n",ror30);
		}
		
		if(getOperandValue(instruction.operand[1],childPID) == 14)
		{
			//printf("round %d ror1 done\n",sha256_roundCount);
			shadowRegisters[getregID(instruction.operand[0].base)].state = ROR1_DONE;
			return 0;
		}
		else
		if(readOperandShadow(instruction.operand[0]) == XOR1_DONE)
		{
			//printf("round %d ror2 done\n",sha256_roundCount);
			shadowRegisters[getregID(instruction.operand[0].base)].state = ROR2_DONE;
			return 0;
		}
		else
		if(readOperandShadow(instruction.operand[0]) == XOR2_DONE)
		{
			//printf("round %d ror3 done\n",sha256_roundCount);
			shadowRegisters[getregID(instruction.operand[0].base)].state = ROR3_DONE;
			return 0;
		}
		else
		if(getOperandValue(instruction.operand[1],childPID) == 6)
		{
			//printf("round %d ror1 done\n",sha256_roundCount);
			shadowRegisters[getregID(instruction.operand[0].base)].state = ROR1_DONE;
			return 0;
		}
		else
		if(getOperandValue(instruction.operand[1],childPID) == 25)
		{
			//printf("round %d ror3 done\n",sha256_roundCount);
			shadowRegisters[getregID(instruction.operand[0].base)].state = ROR3_DONE;
			return 0;
		}
		else
		if(getOperandValue(instruction.operand[1],childPID) == 11)
		{
			//printf("round %d ror2 done\n",sha256_roundCount);
			shadowRegisters[getregID(instruction.operand[0].base)].state = ROR2_DONE;
			return 0;
		}
	}
	else
	if(instruction.mnemonic == UD_Ixor)
	{
		if(readOperandShadow(instruction.operand[1]) == ROR1_DONE || readOperandShadow(instruction.operand[0]) == ROR1_DONE)
		{
			int targetReg;
			if(readOperandShadow(instruction.operand[1]) == ROR1_DONE)
				targetReg = 1;
			if(readOperandShadow(instruction.operand[0]) == ROR1_DONE)
				targetReg = 0;
			shadowRegisters[getregID(instruction.operand[targetReg].base)].state = XOR1_DONE;
			//printf("round %d xor1 done\n",sha256_roundCount);
			return 0;
		}
		else
		if(readOperandShadow(instruction.operand[1]) == ROR2_DONE || readOperandShadow(instruction.operand[0]) == ROR2_DONE)
		{
			int targetReg;
			if(readOperandShadow(instruction.operand[1]) == ROR2_DONE)
				targetReg = 1;
			if(readOperandShadow(instruction.operand[0]) == ROR2_DONE)
				targetReg = 0;
			shadowRegisters[getregID(instruction.operand[targetReg].base)].state = XOR2_DONE;
			//printf("round %d xor2 done\n",sha256_roundCount);
			return 0;
		}
	}
	else
	if(instruction.mnemonic == UD_Irol)
	{
		if(getOperandValue(instruction.operand[1],childPID) == 5)
		{
			rol5++;
			printf("rol5 done %d time(s)\n",rol5);
		}
		else if(getOperandValue(instruction.operand[1],childPID) == 9)
		{
			rol9++;
			printf("rol9 done %d time(s)\n",rol9);
		}
		else if(getOperandValue(instruction.operand[1],childPID) == 25)
		{
			rol25++;
			printf("rol25 done %d time(s)\n",rol25);
		}
		else if(getOperandValue(instruction.operand[1],childPID) == 30)
		{
			rol30++;
			printf("rol30 done %d time(s)\n",rol30);
		}
		/*if(getOperandValue(instruction.operand[1],childPID) == 5)
		{
			printf("beginning of sha1 %d round\n",sha1_roundCount);
			sha1_roundCount++;
		}*/
	}
	else
	if(instruction.mnemonic == UD_Iadd)
	{
		if(readOperandShadow(instruction.operand[1]) == ROR3_DONE || readOperandShadow(instruction.operand[0]) == ROR3_DONE)
		{
			shadowRegisters[getregID(instruction.operand[0].base)].state = ADDL_DONE;
			//printf("sha 256 %d round detected\n",sha256_roundCount);
			sha256_roundCount++;
			return 0;
		}
		else
		if(readOperandShadow(instruction.operand[1]) == XOR2_DONE || readOperandShadow(instruction.operand[0]) == XOR2_DONE)
		{
			shadowRegisters[getregID(instruction.operand[0].base)].state = ADDL_DONE;
			//printf("sha 256 %d round detected\n",sha256_roundCount);
			sha256_roundCount++;
			return 0;
		}
	}
	else
	{
		int destState = START;
		if(instruction.operand[1].type != UD_NONE)
		{
			if(instruction.operand[1].type == UD_OP_REG)
			{
				destState = shadowRegisters[getregID(instruction.operand[1].base)].state;
			}
		}
		if(instruction.operand[0].type != UD_NONE)
		{
			if(instruction.operand[0].type == UD_OP_REG)
			{
				shadowRegisters[getregID(instruction.operand[0].base)].state = destState;
			}
		}
	}
	return 0;
}

bool isThisInstructionTraced(ud_t instruction)
{
	int i = 0;
	while(instruction.operand[i].type != UD_NONE)
	{
		if(readOperandShadow(instruction.operand[i]) == TRACE_THIS)
			return true;
		i++;
	}
	return false;
}
