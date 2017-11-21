# MAKEFILE

EXE = 1 

all: 1 desCBC

1: 1.c 1.h 
	gcc 1.c -o 1 -Wall -O2 -lm -g

desCBC: desCBC.c desCBC.h 
	gcc desCBC.c -o descCBC -Wall -02 -lm -g

clean:
	rm *.o *~ $(EXE)
