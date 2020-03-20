#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "../header/InstructionOperandHandler.h"
#include "../header/ShadowRegister.h"

void readInstructionExecutedByChild(int childPID,long *instruction)
{
	struct user_regs_struct regs;
	ptrace(PTRACE_GETREGS,childPID,NULL,&regs);
	*instruction=ptrace(PTRACE_PEEKTEXT,childPID,regs.eip,NULL);
}

void decodeInstruction(ud_t *ud_obj,long ins)
{
	ud_init(ud_obj);
	ud_set_input_buffer(ud_obj, (unsigned char *)&ins, 32);
	ud_set_mode(ud_obj, 32);
	ud_set_syntax(ud_obj, UD_SYN_INTEL);
	ud_disassemble(ud_obj);
}

int readOperandValue(ud_operand_t oprand,int childPID)
{
	return getOperandValue(oprand,childPID);
}

int main(int argc,char *argv[])
{
    int child;
	if(argc <= 1)
	{
		//need to write this
		return -1;
	}
	int i,j=0;
	char **args=NULL;
	if(argc>1)
	{
		args = (char **)malloc((argc)*sizeof(char *));
		for(i=1;i<argc;i++)
		{
			args[j]=(char *)malloc(strlen(argv[i]));
			args[j]=argv[i];
			j++;
		}
		args[j]=NULL;
	}
	child=fork();
    if(child==0)
    {
		ptrace(PTRACE_TRACEME,child,NULL,NULL);
        execvp(argv[1],args);
		printf("error in executing:%s\n",argv[1]);
    }
    else
    {
		InitializeShadowRegisters();
		long ins;
		int status;
		while(1)
		{
			wait(&status);
			if(WIFEXITED(status))
				break;

			//read the instruction from child process
			readInstructionExecutedByChild(child,&ins);
			
			//convert it to ud_t object
			ud_t ud_obj;
			decodeInstruction(&ud_obj,ins);
			const char *instruction = ud_insn_asm(&ud_obj);
			
			/*if(strstr(instruction,"sar")!=NULL)
				printf("%s\n",instruction);*/
			
			if(isThisInstructionTraced(ud_obj))
				printf("tr:%s\n",instruction);
			
			if(CheckState(ud_obj,child,instruction)==1)
				printf("%s\n",instruction);
			
			//continue to next step
			ptrace(PTRACE_SINGLESTEP,child,NULL,NULL);
		}
    }
}
