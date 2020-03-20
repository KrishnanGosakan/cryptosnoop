#include "CryptoState.h"

struct CrypoState shadowRegisters[139];

void InitializeShadowRegisters();

int getregID(int reg);

int readOperandShadow(ud_operand_t oprand);
