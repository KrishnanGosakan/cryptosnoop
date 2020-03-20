gcc -std=c99 ../src/ShadowRegister.c -o shadowregister.o -c

gcc -std=c99 ../src/InstructionOperandHandler.c -o ioh.o -c

gcc -std=c99 ../src/CryptoState.c -o cryptostate.o -c

gcc -std=c99 ../src/insdb.c -o insdb.o -c

gcc -o CryptoLogger shadowregister.o ioh.o cryptostate.o insdb.o -ludis86
