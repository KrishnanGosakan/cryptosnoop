#include <udis86.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>

int readRegistry(int regID,int childPID);

int getOperandValue(ud_operand_t oprand,int childPID);
