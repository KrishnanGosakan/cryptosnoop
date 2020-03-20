#include "../header/InstructionOperandHandler.h"

int readRegistry(int regID,int childPID)
{
	struct user_regs_struct regs;
	ptrace(PTRACE_GETREGS,childPID,NULL,&regs);

	switch(regID)
	{
		case UD_R_EAX:
			return regs.orig_eax;
		case UD_R_ECX:
			return regs.ecx;
		case UD_R_EDX:
			return regs.edx;
		case UD_R_EBX:
			return regs.ebx;
		case UD_R_ESP:
			return regs.esp;
		case UD_R_EBP:
			return regs.ebp;
		case UD_R_ESI:
			return regs.esi;
		case UD_R_EDI:
			return regs.edi;
		default:
			return 0;
	}
}

int getOperandValue(ud_operand_t oprand,int childPID)
{
	switch(oprand.type)
	{
		case UD_OP_MEM:
			//not handled now
			break;
		case UD_OP_REG:
			return readRegistry(oprand.base,childPID);
		case UD_OP_PTR:
			//not handled now
			break;
		case UD_OP_IMM:
			return oprand.lval.sdword;
	}
	return UD_NONE;
}
