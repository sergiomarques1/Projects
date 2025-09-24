CC = gcc
FLAGS = -Wall -Wextra -pthread
LDFLAGS = -lcrypto

all:		DEIChain TxGen

clean:		
			rm -f *.o
			rm -f DEIChain TxGen
			> DEIChain_log.txt


DEIChain:	DEIChain.o
			${CC} ${FLAGS} DEIChain.o pow.o -o $@ $(LDFLAGS)

TxGen:		generator.o
			${CC} ${FLAGS} generator.o -o $@
			

.c.o:		
			${CC} ${FLAGS} $< -c -o $@
			
			

############################

DEIChain.o:		DEIChain.h DEIChain.c

pow.o:			pow.h pow.c

generator.o:	DEIChain.h generator.c

DEIChain:		DEIChain.o pow.o

TxGen:			generator.o
