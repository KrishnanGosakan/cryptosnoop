#include "../header/ShadowRegister.h"

void InitializeShadowRegisters()
{
	int i;
	for(i=0;i<139;i++)
		shadowRegisters[i].state = START;
}

int readOperandShadow(ud_operand_t oprand)
{
	switch(oprand.type)
	{
		case UD_OP_MEM:
			//not handled now
			break;
		case UD_OP_REG:
			return shadowRegisters[getregID(oprand.base)].state;
		case UD_OP_PTR:
			//not handled now
			break;
		case UD_OP_IMM:
			break;
	}
	return START;
}


int getregID(int reg)
{
	switch(reg)
	{

	/* 8 bit GPRs */

		case UD_R_AL:
			return 0;
		case UD_R_CL:
			return 1;
		case UD_R_DL:
			return 2;
		case UD_R_BL:
			return 3;
		case UD_R_AH:
			return 4;
		case UD_R_CH:
			return 5;
		case UD_R_DH:
			return 6;
		case UD_R_BH:
			return 7;
		case UD_R_SPL:
			return 8;
		case UD_R_BPL:
			return 9;
		case UD_R_SIL:
			return 10;
		case UD_R_DIL:
			return 11;
		case UD_R_R8B:
			return 12;
		case UD_R_R9B:
			return 13;
		case UD_R_R10B:
			return 14;
		case UD_R_R11B:
			return 15;
		case UD_R_R12B:
			return 16;
		case UD_R_R13B:
			return 17;
		case UD_R_R14B:
			return 18;
		case UD_R_R15B:
			return 19;

	/* 16 bit GPRs */

		case UD_R_AX:
			return 20;
		case UD_R_CX:
			return 21;
		case UD_R_DX:
			return 22;
		case UD_R_BX:
			return 23;
		case UD_R_SP:
			return 24;
		case UD_R_BP:
			return 25;
		case UD_R_SI:
			return 26;
		case UD_R_DI:
			return 27;
		case UD_R_R8W:
			return 28;
		case UD_R_R9W:
			return 29;
		case UD_R_R10W:
			return 30;
		case UD_R_R11W:
			return 31;
		case UD_R_R12W:
			return 32;
		case UD_R_R13W:
			return 33;
		case UD_R_R14W:
			return 34;
		case UD_R_R15W:
			return 35;

	/* 32 bit GPRs */

		case UD_R_EAX:
			return 36;
		case UD_R_ECX:
			return 37;
		case UD_R_EDX:
			return 38;
		case UD_R_EBX:
			return 39;
		case UD_R_ESP:
			return 40;
		case UD_R_EBP:
			return 41;
		case UD_R_ESI:
			return 42;
		case UD_R_EDI:
			return 43;
		case UD_R_R8D:
			return 44;
		case UD_R_R9D:
			return 45;
		case UD_R_R10D:
			return 46;
		case UD_R_R11D:
			return 47;
		case UD_R_R12D:
			return 48;
		case UD_R_R13D:
			return 49;
		case UD_R_R14D:
			return 50;
		case UD_R_R15D:
			return 51;

	/* 64 bit GPRs */

		case UD_R_RAX:
			return 52;
		case UD_R_RCX:
			return 53;
		case UD_R_RDX:
			return 54;
		case UD_R_RBX:
			return 55;
		case UD_R_RSP:
			return 56;
		case UD_R_RBP:
			return 57;
		case UD_R_RSI:
			return 58;
		case UD_R_RDI:
			return 59;
		case UD_R_R8:
			return 60;
		case UD_R_R9:
			return 61;
		case UD_R_R10:
			return 62;
		case UD_R_R11:
			return 63;
		case UD_R_R12:
			return 64;
		case UD_R_R13:
			return 65;
		case UD_R_R14:
			return 66;
		case UD_R_R15:
			return 67;

	/* segment registers */

		case UD_R_ES:
			return 68;
		case UD_R_CS:
			return 69;
		case UD_R_SS:
			return 70;
		case UD_R_DS:
			return 71;
		case UD_R_FS:
			return 72;
		case UD_R_GS:
			return 73;

	/* control registers*/

		case UD_R_CR0:
			return 74;
		case UD_R_CR1:
			return 75;
		case UD_R_CR2:
			return 76;
		case UD_R_CR3:
			return 77;
		case UD_R_CR4:
			return 78;
		case UD_R_CR5:
			return 79;
		case UD_R_CR6:
			return 80;
		case UD_R_CR7:
			return 81;
		case UD_R_CR8:
			return 82;
		case UD_R_CR9:
			return 83;
		case UD_R_CR10:
			return 84;
		case UD_R_CR11:
			return 85;
		case UD_R_CR12:
			return 86;
		case UD_R_CR13:
			return 87;
		case UD_R_CR14:
			return 88;
		case UD_R_CR15:
			return 89;

	/* debug registers */

		case UD_R_DR0:
			return 90;
		case UD_R_DR1:
			return 91;
		case UD_R_DR2:
			return 92;
		case UD_R_DR3:
			return 93;
		case UD_R_DR4:
			return 94;
		case UD_R_DR5:
			return 95;
		case UD_R_DR6:
			return 96;
		case UD_R_DR7:
			return 97;
		case UD_R_DR8:
			return 98;
		case UD_R_DR9:
			return 99;
		case UD_R_DR10:
			return 100;
		case UD_R_DR11:
			return 101;
		case UD_R_DR12:
			return 102;
		case UD_R_DR13:
			return 103;
		case UD_R_DR14:
			return 104;
		case UD_R_DR15:
			return 105;

	/* mmx registers */

		case UD_R_MM0:
			return 106;
		case UD_R_MM1:
			return 107;
		case UD_R_MM2:
			return 108;
		case UD_R_MM3:
			return 108;
		case UD_R_MM4:
			return 110;
		case UD_R_MM5:
			return 111;
		case UD_R_MM6:
			return 112;
		case UD_R_MM7:
			return 113;

	/* x87 registers */

		case UD_R_ST0:
			return 114;
		case UD_R_ST1:
			return 115;
		case UD_R_ST2:
			return 116;
		case UD_R_ST3:
			return 117;
		case UD_R_ST4:
			return 118;
		case UD_R_ST5:
			return 119;
		case UD_R_ST6:
			return 120;
		case UD_R_ST7:
			return 121;

	/* extended multimedia registers */

		case UD_R_XMM0:
			return 122;
		case UD_R_XMM1:
			return 123;
		case UD_R_XMM2:
			return 124;
		case UD_R_XMM3:
			return 125;
		case UD_R_XMM4:
			return 126;
		case UD_R_XMM5:
			return 127;
		case UD_R_XMM6:
			return 128;
		case UD_R_XMM7:
			return 129;
		case UD_R_XMM8:
			return 130;
		case UD_R_XMM9:
			return 131;
		case UD_R_XMM10:
			return 132;
		case UD_R_XMM11:
			return 133;
		case UD_R_XMM12:
			return 134;
		case UD_R_XMM13:
			return 135;
		case UD_R_XMM14:
			return 136;
		case UD_R_XMM15:
			return 137;

		case UD_R_RIP:
			return 138;

		default:
			return 0;
	}
}
