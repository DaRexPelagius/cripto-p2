# MAKEFILE

EXE = 1 

all: 1 desCBC desECB NL_DES

1: 1.c 1.h 
	gcc 1.c -o 1 -Wall -O2 -lm -g

desCBC: desCBC.c desCBC.h 
	gcc desCBC.c -o desCBC -Wall -O2 -lm -g

desECB: desECB.c desECB.h 
	gcc desECB.c -o desECB -Wall -O2 -lm -g
	
NL_DES: NL_DES.c NL_DES.h 
	gcc NL_DES.c -o NL_DES -Wall -O2 -lm -g	
	
clean:
	rm *.o *~ $(EXE)
