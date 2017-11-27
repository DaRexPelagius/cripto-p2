# MAKEFILE

EXE = 1 

all: 1 desCBC desECB 3a 3b 3c

1: 1.c 1.h 
	gcc 1.c -o 1 -Wall -O2 -lm -g

desCBC: desCBC.c desCBC.h 
	gcc desCBC.c -o desCBC -Wall -O2 -lm -g

desECB: desECB.c desECB.h 
	gcc desECB.c -o desECB -Wall -O2 -lm -g
	
3a: 3a.c 3a.h 
	gcc 3a.c -o 3a -Wall -O2 -lm -g	
		
3b: 3b.c 3b.h 
	gcc 3b.c -o 3b -Wall -O2 -lm -g	
	
3c: 3c.c 3c.h 
	gcc 3c.c -o 3c -Wall -O2 -lm -g	
	
4a: 4a.c 4a.h 
	gcc 4a.c -o 4a -Wall -O2 -lm -g	
	
clean:
	rm 1 desCBC desECB 3a 3b 3c
