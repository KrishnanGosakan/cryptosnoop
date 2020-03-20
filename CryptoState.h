#include <udis86.h>
#include <stdbool.h>

struct CrypoState
{
	int state;
};

enum
{
START,
ROR1_DONE,
XOR1_DONE,
ROR2_DONE,
XOR2_DONE,
ROR3_DONE,
ADDL_DONE,
TRACE_THIS
} PossibleStates;

int CheckState(ud_t instruction,int childPID,const char *instr);

bool isThisInstructionTraced(ud_t instruction);
