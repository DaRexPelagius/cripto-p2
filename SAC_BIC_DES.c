#include "SAC_BIC_DES.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

int MATRIX_AUX2[ROWS_PER_SBOX][BITS_OUT_BOX][BITS_OUT_BOX] = {
	{ 
		{1, 1, 1, 0},
		{1, 1, 0, 0},
		{1, 0, 0, 0},
		{1, 0, 0, 1}
	},
	{ 
		{1, 1, 0, 1},
		{0, 0, 0, 1},
		{0, 1, 0, 0},
		{0, 0, 1, 0}
	},
	{ 
		{1, 1, 0, 0},
		{0, 1, 0, 0},
		{1, 0, 1, 1},
		{0, 0, 0, 1}
	},
	{ 
		{1, 1, 1, 1},
		{1, 1, 1, 0},
		{1, 1, 0, 0},
		{1, 0, 0, 0}
	}
};

int* MATRIX_AUX1[ROWS_PER_SBOX][BITS_OUT_BOX] = {
	{ MATRIX_AUX2[0][0], MATRIX_AUX2[0][1], MATRIX_AUX2[0][2], MATRIX_AUX2[0][3] }, 
	{ MATRIX_AUX2[1][0], MATRIX_AUX2[1][1], MATRIX_AUX2[1][2], MATRIX_AUX2[1][3] }, 
	{ MATRIX_AUX2[2][0], MATRIX_AUX2[2][1], MATRIX_AUX2[2][2], MATRIX_AUX2[2][3] }, 
	{ MATRIX_AUX2[3][0], MATRIX_AUX2[3][1], MATRIX_AUX2[3][2], MATRIX_AUX2[3][3] }, 
};

int** MATRIX[ROWS_PER_SBOX] = {
	MATRIX_AUX1[0], MATRIX_AUX1[1], MATRIX_AUX1[2], MATRIX_AUX1[3]
};

int TRANSLATION[BITS_OUT_BOX] = {
	1, 1, 0, 0
};

int POLS_AUX[2][BITS_OUT_BOX+1] = {
	{ 1, 1, 1, 1, 1 },
	{ 1, 1, 0, 0, 1 },
};

int* POLS[2] = {
	POLS_AUX[0], POLS_AUX[1]
};


/* PC1 "permutation" */
static const unsigned short PC1[BITS_IN_PC1] = { 
	57, 49, 41, 33, 25, 17, 9,
	1, 58, 50, 42, 34, 26, 18,
	10, 2, 59, 51, 43, 35, 27,
	19, 11, 3, 60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15,
	7, 62, 54, 46, 38, 30, 22,
	14, 6, 61, 53, 45, 37, 29,
	21, 13, 5, 28, 20, 12, 4
};

/* PC2 "permutation" */
static const unsigned short PC2[BITS_IN_PC2] = {
	14, 17, 11, 24, 1, 5,
	3, 28, 15, 6, 21, 10,
	23, 19, 12, 4, 26, 8,
	16, 7, 27, 20, 13, 2,
	41, 52, 31, 37, 47, 55,
	30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53,
	46, 42, 50, 36, 29, 32
};

/* Shifts in each of the key halves in each round (for encryption) */
static const unsigned short ROUND_SHIFTS[NUM_ROUNDS] = {
	1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

/* Shifts in each of the key halves in each round (for decryption) */
static const unsigned short ROUND_SHIFTS_DEC[NUM_ROUNDS] = {
	0, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

/* IP permutation */
static const unsigned short IP[BITS_IN_IP] = {
	58, 50, 42, 34, 26, 18, 10, 2,
	60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14, 6,
	64, 56, 48, 40, 32, 24, 16, 8,
	57, 49, 41, 33, 25, 17, 9, 1,
	59, 51, 43, 35, 27, 19, 11, 3,
	61, 53, 45, 37, 29, 21, 13, 5,
	63, 55, 47, 39, 31, 23, 15, 7
};

/* IP inverse */
static const unsigned short IP_INV[BITS_IN_IP] = {
	40, 8, 48, 16, 56, 24, 64, 32,
	39, 7, 47, 15, 55, 23, 63, 31,
	38, 6, 46, 14, 54, 22, 62, 30,
	37, 5, 45, 13, 53, 21, 61, 29,
	36, 4, 44, 12, 52, 20, 60, 28,
	35, 3, 43, 11, 51, 19, 59, 27,
	34, 2, 42, 10, 50, 18, 58, 26,
	33, 1, 41, 9, 49, 17, 57, 25
};

/* E expansion */
static const unsigned short E[BITS_IN_E] = {
	32, 1, 2, 3, 4, 5,
	4, 5, 6, 7, 8, 9,
	8, 9, 10, 11, 12, 13,
	12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21,
	20, 21, 22, 23, 24, 25,
	24, 25, 26, 27, 28, 29,
	28, 29, 30, 31, 32, 1
};

/* P permutation */
static const unsigned short P[BITS_IN_P] = {
	16, 7, 20, 21,
	29, 12, 28, 17,
	1, 15, 23, 26,
	5, 18, 31, 10,
	2, 8, 24, 14,
	32, 27, 3, 9,
	19, 13, 30, 6,
	22, 11, 4, 25
};

/* 32-bit swap */
static const unsigned short SWAP[BITS_IN_SWAP] = {
	33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48,
	49, 50, 51, 52, 53, 54, 55, 56,
	57, 58, 59, 60, 61, 62, 63, 64,
	1, 2, 3, 4, 5, 6, 7, 8,
	9, 10, 11, 12, 13, 14, 15, 16,
	17, 18, 19, 20, 21, 22, 23, 24,
	25, 26, 27, 28, 29, 30, 31, 32
};

/* S-boxes */
static const unsigned short S_BOXES[NUM_S_BOXES][ROWS_PER_SBOX][COLUMNS_PER_SBOX] = {
	{	{ 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7 },
		{ 0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8 },
		{ 4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0 },
		{ 15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13 } },
	{
		{ 15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10 },
		{ 3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5 },
		{ 0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15 },
		{ 13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9 }	},

	{	{ 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8 },
		{ 13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1 },
		{ 13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7 },
		{ 1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12 }	},

	{	{ 7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15 },
		{ 13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9 },
		{ 10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4 },
		{ 3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14 }	},
	{
		{ 2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9 },
		{ 14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6 },
		{ 4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14 },
		{ 11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3 }	},
	{	
		{ 12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11 },
		{ 10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8 },
		{ 9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6 },
		{ 4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13 }	},
	{
 		{ 4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1 },
		{ 13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6 },
		{ 1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2 },
		{ 6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12 }	},
	{
		{ 13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7 },
		{ 1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2 },
		{ 7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8 },
		{ 2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11 }	}


};



int main(int argc, char** argv) {

	int criterionFlag,nTests,flagOutput;
	char outputFileName[MAX_NAME_LENGTH];
	FILE* outputFile = NULL;
	SAC SACstatistics;
	BIC BICstatistics;

	/* Gets command-line arguments */
	switch (getArgs(argc,argv,&criterionFlag,&nTests,outputFileName,&flagOutput)) {
		case ERR_N_ARGS:
			printUsage("ERROR: Wrong number of command-line arguments\n");
			return -1;
		case ERR_FLAGS:
			printUsage("ERROR: Wrong flags\n");
			return -1;
	}

	/* Validates command-line arguments */
	if (nTests < 1) {
		fprintf(stderr,"ERROR: The number of tests must be greater than 0\n");
		return -1;
	}

	if (flagOutput) {
		if ((outputFile = fopen(outputFileName,"w")) == NULL) {
			fprintf(stderr,"ERROR: Unable to open %s\n",outputFileName);
			return -1;
		}
	}
	else
		outputFile = stdout;

	/* SAC/BIC measurement */
	if (criterionFlag == SAC_FLAG) {
		measureSAC(&SACstatistics,nTests);
		printSAC(outputFile,&SACstatistics);
	}
	else if (criterionFlag == BIC_FLAG) {
		measureBIC(&BICstatistics,nTests);
		printBIC(outputFile,&BICstatistics);
	}

	if (outputFile && (outputFile != stdout))
		fclose(outputFile);

	return 0;
}


void printUsage(char* message) {

	if (message)
		fprintf(stderr,"%s",message);

	fprintf(stderr,"Syntax: ./desSAC_BIC {-S|-B} {-t nTests} [-o outputFile]\n");
	fprintf(stderr,"- S:\t\tStrict Avalanche Criterion measurement\n");
	fprintf(stderr,"- B:\t\tBit Independence Criterion measurement\n");
	fprintf(stderr,"- nTests:\tNumber of tests to perform for the measurements\n");
	fprintf(stderr,"- outputFile:\tOutput file (default: stdout)\n");
}


int getArgs(int nArgs, char** args, int* criterionFlag, int* nTests, char* outputFileName, int* flagOutput) {

	if ((nArgs != 4) && (nArgs != 6))
		return ERR_N_ARGS;

	if (getCriterionFlag(nArgs,args,criterionFlag) != 1)
		return ERR_FLAGS;
	if (getInteger(nArgs,args,nTests,"-t",2) != 1)
		return ERR_FLAGS;

	if (nArgs == 4)
		*flagOutput = 0;
	else if (((*flagOutput) = getString(nArgs,args,outputFileName,"-o",2)) != 1)
		return ERR_FLAGS;

	return 0;
}

int getCriterionFlag(int nArgs, char** args, int* criterionFlag) {

	int i;
	int found = 0;

	for (i=1; i<nArgs; i++)
		if ((strncmp(args[i],"-S",2) == 0) && (strlen(args[i]) == 2)) {
			if (found)
				return -1;
			else {
				*criterionFlag = SAC_FLAG;
				found = 1;
			}
		}
		else if (strncmp(args[i],"-B",2) == 0) {
			if (found)
				return -1;
			else {
				*criterionFlag = BIC_FLAG;
				found = 1;
			}
		}

	return found;
}

int getInteger(int nArgs, char** args, int* integer, char* flag, int flagLength) {

	int i;
	int found = 0;

	for (i=1; i<=nArgs-2; i++)
		if ((strncmp(args[i],flag,flagLength) == 0) && (strlen(args[i]) == flagLength)) {
			if (found)
				return -1;
			else {
				*integer = atoi(args[i+1]);
				if ((*integer == 0) && !isdigit(args[i+1][0]))
					return -1;
				found = 1;
			}
		}

	return found;
}


int getString(int nArgs, char** args, char* string, char* flag, int flagLength) {

	int i;
	int found = 0;

	for (i=1; i<=nArgs-2; i++)
		if ((strncmp(args[i],flag,flagLength) == 0) && (strlen(args[i]) == flagLength)) {
			if (found)
				return -1;
			else {
				strcpy(string,args[i+1]);
				found = 1;
			}
		}

	return found;
}

void measureSAC(SAC* statistics, int nTests) {

	unsigned short*** Sboxes = NULL;
	unsigned long int*** bitChangesAcc = NULL;
	int i, j, k;

	srand(time(NULL));

	/* Initialises the counter */
	bitChangesAcc = allocSACacc();
	for (i=0; i<NUM_S_BOXES; i++)
		for (j=0; j<SBOX_OUTPUT_BITS; j++)
			for (k=0; k<SBOX_INPUT_BITS; k++)
				bitChangesAcc[i][j][k] = 0;

	/* Measures SAC */
	Sboxes = allocSboxes();
	for (i=0; i<nTests; i++)
		singleSAC(Sboxes,bitChangesAcc);
	freeSboxes(Sboxes);

	/* Computes the average */
	for (i=0; i<NUM_S_BOXES; i++)
		for (j=0; j<SBOX_OUTPUT_BITS; j++)
			for (k=0; k<SBOX_INPUT_BITS; k++)
				statistics->condProb[i][j][k] = (bitChangesAcc[i][j][k])*(1./nTests);
	freeSACacc(bitChangesAcc);
}

void printSAC(FILE* outputFile, SAC* statistics) {

	int i,j,k;

	for (i=0; i<NUM_S_BOXES; i++) {
		fprintf(outputFile,"S-box %d:\n",i+1);
		for (j=0; j<SBOX_OUTPUT_BITS; j++)
			for (k=0; k<SBOX_INPUT_BITS; k++)
				fprintf(outputFile,"P(¬b%d|¬a%d) =\t%.2f\n",j+1,k+1,statistics->condProb[i][j][k]);
	}
}

/* Indices pairs for the S-boxes output bits */
static const unsigned short OUTPUT_PAIRS[SBOX_OUTPUT_PAIRS][2] = {
	{ 0, 1 },
	{ 0, 2 },
	{ 0, 3 },
	{ 1, 2 },
	{ 1, 3 },
	{ 2, 3 }
};


void measureBIC(BIC* statistics, int nTests) {

	unsigned short*** Sboxes = NULL;
	unsigned long int*** pairChangesAcc = NULL;
	int i, j, k;
	float _p1_p2, _p1p2, p1_p2, p1p2, p1, _p1, p2, _p2;

	srand(time(NULL));

	/* Initialises the counter */
	pairChangesAcc = allocBICacc();
	for (i=0; i<NUM_S_BOXES; i++)
		for (j=0; j<4*SBOX_OUTPUT_PAIRS; j++)
			for (k=0; k<SBOX_INPUT_BITS; k++)
				pairChangesAcc[i][j][k] = 0;

	/* Measures BIC */
	Sboxes = allocSboxes();
	for (i=0; i<nTests; i++)
		singleBIC(Sboxes,pairChangesAcc);
	freeSboxes(Sboxes);

	/* Computes the conditioned probabilities from the joint frequencies */
	for (i=0; i<NUM_S_BOXES; i++)
		for (j=0; j<SBOX_OUTPUT_PAIRS; j++)
			for (k=0; k<SBOX_INPUT_BITS; k++) {
				_p1_p2 = pairChangesAcc[i][4*j][k] * (1./nTests);
				_p1p2 = pairChangesAcc[i][4*j+1][k] * (1./nTests);
				p1_p2 = pairChangesAcc[i][4*j+2][k] * (1./nTests);
				p1p2 = pairChangesAcc[i][4*j+3][k] * (1./nTests);
				_p1 = _p1_p2 + _p1p2;
				p1 = p1_p2 + p1p2;
				_p2 = _p1_p2 + p1_p2;
				p2 = _p1p2 + p1p2;

				statistics->condProb[i][8*j][k] = _p1_p2 * (1./_p2);
				statistics->condProb[i][8*j+1][k] = _p1p2 * (1./p2);
				statistics->condProb[i][8*j+2][k] = p1_p2 * (1./_p2);
				statistics->condProb[i][8*j+3][k] = p1p2 * (1./p2);
				statistics->condProb[i][8*j+4][k] = _p1_p2 * (1./_p1);
				statistics->condProb[i][8*j+5][k] = p1_p2 * (1./p1);
				statistics->condProb[i][8*j+6][k] = _p1p2 * (1./_p1);
				statistics->condProb[i][8*j+7][k] = p1p2 * (1./p1);
			}
	freeBICacc(pairChangesAcc);
}

void printBIC(FILE* outputFile, BIC* statistics) {

	int i,j,k;

	for (i=0; i<NUM_S_BOXES; i++) {
		fprintf(outputFile,"S-box %d:\n",i+1);
		for (j=0; j<SBOX_OUTPUT_PAIRS; j++)
			for (k=0; k<SBOX_INPUT_BITS; k++) {
				fprintf(outputFile,"P(¬b%d|¬b%d,¬a%d) =\t%.2f\n",OUTPUT_PAIRS[j][0]+1,OUTPUT_PAIRS[j][1]+1,k+1,statistics->condProb[i][8*j][k]);
				fprintf(outputFile,"P(¬b%d|b%d,¬a%d) =\t%.2f\n",OUTPUT_PAIRS[j][0]+1,OUTPUT_PAIRS[j][1]+1,k+1,statistics->condProb[i][8*j+1][k]);
				fprintf(outputFile,"P(¬b%d|¬b%d,¬a%d) =\t%.2f\n",OUTPUT_PAIRS[j][1]+1,OUTPUT_PAIRS[j][0]+1,k+1,statistics->condProb[i][8*j+4][k]);
				fprintf(outputFile,"P(¬b%d|b%d,¬a%d) =\t%.2f\n",OUTPUT_PAIRS[j][1]+1,OUTPUT_PAIRS[j][0]+1,k+1,statistics->condProb[i][8*j+5][k]);
			}
	}
}

unsigned long int*** allocSACacc() {

	unsigned long int*** SACacc;
	int i, j;

	SACacc = (unsigned long int***)malloc(NUM_S_BOXES*sizeof(unsigned long int**));
	for (i=0; i<NUM_S_BOXES; i++) {
		SACacc[i] = (unsigned long int**)malloc(SBOX_OUTPUT_BITS*sizeof(unsigned long int*));
		for (j=0; j<SBOX_OUTPUT_BITS; j++)
			SACacc[i][j] = (unsigned long int*)malloc(SBOX_INPUT_BITS*sizeof(unsigned long int));
	}

	return SACacc;
}


void freeSACacc(unsigned long int*** SACacc) {

	int i, j;
	
	for (i=0; i<NUM_S_BOXES; i++) {
		for (j=0; j<SBOX_OUTPUT_BITS; j++)
			free(SACacc[i][j]);
		free(SACacc[i]);
	}

	free(SACacc);
	SACacc = NULL;
}

unsigned short*** allocSboxes() {
	
	#ifdef __NEWDES__
		return getNewSboxes();
	#else
		#ifdef __RANDOMDES__
			return getNewRandomSboxes();
		#else
			unsigned short*** Sboxes;
			int i, j;

			Sboxes = (unsigned short***)malloc(NUM_S_BOXES*sizeof(unsigned short**));
			for (i=0; i<NUM_S_BOXES; i++) {
				Sboxes[i] = (unsigned short**)malloc(ROWS_PER_SBOX*sizeof(unsigned short*));
				for (j=0; j<ROWS_PER_SBOX; j++)
					Sboxes[i][j] = (unsigned short*)malloc(COLUMNS_PER_SBOX*sizeof(unsigned short));
			}

			getSboxes(Sboxes);

			return Sboxes;
		#endif
	#endif
}

void singleSAC(unsigned short*** Sboxes, unsigned long int*** bitChangesAcc) {

	DESblock input, output;
	DESblock input2, output2;
	int i, j, k;

	newBlock(&input,SBOX_INPUT_BITS*NUM_S_BOXES);

	/* Changes each possible input bit */
	for (k=0; k<SBOX_INPUT_BITS; k++) {

		copyBlock(&input2,&input,SBOX_INPUT_BITS*NUM_S_BOXES);
		alterSboxInput(&input2,k);

		SBoxGeneral(&output,&input, Sboxes);
		SBoxGeneral(&output2,&input2, Sboxes);

		for (i=0; i<NUM_S_BOXES; i++)
			for (j=0; j<SBOX_OUTPUT_BITS; j++)
				if (output2.block[i*SBOX_OUTPUT_BITS+j+1] != output.block[i*SBOX_OUTPUT_BITS+j+1])
					(bitChangesAcc[i][j][k])++;
	}
}

void freeSboxes(unsigned short*** Sboxes) {

	int i, j;

	for (i=0; i<NUM_S_BOXES; i++) {
		for (j=0; j<ROWS_PER_SBOX; j++)
			free(Sboxes[i][j]);
		free(Sboxes[i]);
	}

	free(Sboxes);
	Sboxes = NULL;
}

void SBoxGeneral(DESblock* new, DESblock* old, short unsigned int*** Sbox) {

	int i, row, col;
	int value;

	for (i=0; i < NUM_S_BOXES; i++) {
		row = 2*old->block[1+6*i] + old->block[1+6*i+5];
		col = 8*old->block[1+6*i+1] + 4*old->block[1+6*i+2] + 2*old->block[1+6*i+3] + old->block[1+6*i+4];
		
		value = Sbox[i][row][col];
		new->block[1+i*4+3] = value%2;
		new->block[1+i*4+2] = (value/2)%2;
		new->block[1+i*4+1] = (value/4)%2;
		new->block[1+i*4] = (value/8)%2;
	}
}


unsigned long int*** allocBICacc() {

	unsigned long int*** BICacc;
	int i, j;

	BICacc = (unsigned long int***)malloc(NUM_S_BOXES*sizeof(unsigned long int**));
	for (i=0; i<NUM_S_BOXES; i++) {
		BICacc[i] = (unsigned long int**)malloc(4*SBOX_OUTPUT_PAIRS*sizeof(unsigned long int*));
		for (j=0; j<4*SBOX_OUTPUT_PAIRS; j++)
			BICacc[i][j] = (unsigned long int*)malloc(SBOX_INPUT_BITS*sizeof(unsigned long int));
	}

	return BICacc;
}

void singleBIC(unsigned short*** Sboxes, unsigned long int*** pairChangesAcc) {

	DESblock input, output;
	DESblock input2, output2;
	int i, j, k;
	int change1, change2;

	newBlock(&input,SBOX_INPUT_BITS*NUM_S_BOXES);

	/* Changes each possible input bit */
	for (k=0; k<SBOX_INPUT_BITS; k++) {

		copyBlock(&input2,&input,SBOX_INPUT_BITS*NUM_S_BOXES);
		alterSboxInput(&input2,k);

		SBoxGeneral(&output,&input, Sboxes);
		SBoxGeneral(&output2,&input2, Sboxes);

		for (i=0; i<NUM_S_BOXES; i++)
			for (j=0; j<SBOX_OUTPUT_PAIRS; j++) {

				/* Check if each of the two output bits has changed */
				if (output2.block[i*SBOX_OUTPUT_BITS+OUTPUT_PAIRS[j][0]+1] != output.block[i*SBOX_OUTPUT_BITS+OUTPUT_PAIRS[j][0]+1])
					change1 = 1;
				else
					change1 = 0;
				if (output2.block[i*SBOX_OUTPUT_BITS+OUTPUT_PAIRS[j][1]+1] != output.block[i*SBOX_OUTPUT_BITS+OUTPUT_PAIRS[j][1]+1])
					change2 = 1;
				else
					change2 = 0;

				/* Increases the corresponding output pair counter */
				if (change1 && change2)
					(pairChangesAcc[i][4*j][k])++;
				else if (change1 && !change2)
					(pairChangesAcc[i][4*j+1][k])++;
				else if (!change1 && change2)
					(pairChangesAcc[i][4*j+2][k])++;
				else
					(pairChangesAcc[i][4*j+3][k])++;
			}
	}
}

void freeBICacc(unsigned long int*** BICacc) {

	int i, j;
	
	for (i=0; i<NUM_S_BOXES; i++) {
		for (j=0; j<4*SBOX_OUTPUT_PAIRS; j++)
			free(BICacc[i][j]);
		free(BICacc[i]);
	}

	free(BICacc);
	BICacc = NULL;
}

void getSboxes(unsigned short*** Sboxes) {

	int box, r, c;

	for (box=0; box<NUM_S_BOXES; box++)
		for (r=0; r<ROWS_PER_SBOX; r++)
			for (c=0; c<COLUMNS_PER_SBOX; c++)
				Sboxes[box][r][c] = S_BOXES[box][r][c];
}

unsigned short*** getNewSboxes() {

	unsigned short*** Sboxes;
	int i, j, k;
	uint8_t value;

	Sboxes = (unsigned short***)malloc(NUM_S_BOXES*sizeof(unsigned short**));
	for (i=0; i<NUM_S_BOXES; i++) {
		Sboxes[i] = (unsigned short**)malloc(ROWS_PER_SBOX*sizeof(unsigned short*));
		for (j=0; j<ROWS_PER_SBOX; j++) {
			Sboxes[i][j] = (unsigned short*)malloc(COLUMNS_PER_SBOX*sizeof(unsigned short));
			for (k=0; k < COLUMNS_PER_SBOX; k++) {
				value = (((j >> 1)%2) << 5) + (k << 1) + j%2;
				Sboxes[i][j][k] = codeDirect(value, j, i);
			}
		}
	}

	return Sboxes;
}

unsigned short*** getNewRandomSboxes() {

	unsigned short*** Sboxes;
	int i, j, k;
	int mark[16];
	int temp;

	Sboxes = (unsigned short***)malloc(NUM_S_BOXES*sizeof(unsigned short**));
	for (i=0; i<NUM_S_BOXES; i++) {
		Sboxes[i] = (unsigned short**)malloc(ROWS_PER_SBOX*sizeof(unsigned short*));
		for (j=0; j<ROWS_PER_SBOX; j++) {
			Sboxes[i][j] = (unsigned short*)malloc(COLUMNS_PER_SBOX*sizeof(unsigned short));
			
			for (k=0; k < 16; ++k) mark[k] = 1;
			
			for (k=0; k < COLUMNS_PER_SBOX; k++) {
				while (1) {
					temp = rand()%16;
					if (mark[temp]) {
						mark[temp] = 0;
						break;
					}
				}
				Sboxes[i][j][k] = temp;
			}
		}
	}
	
	return Sboxes;
}

void newBlock(DESblock* b, int blockSize) {

	int i;

	for (i=1; i<=blockSize; i++)
		b->block[i] = randomInt(0,1);
}

void copyBlock(DESblock* new, DESblock* old, int length) {

	int i;

	for (i=1; i <= length; i++)
		new->block[i] = old->block[i];
}

void alterSboxInput(DESblock* inputBlock, int bit) {

	int i;

	for (i=bit; i<SBOX_INPUT_BITS*NUM_S_BOXES; i+=SBOX_INPUT_BITS) {
		if (inputBlock->block[i+1] == 0)
			inputBlock->block[i+1] = 1;
		else
			inputBlock->block[i+1] = 0;
	}
}

int randomInt(int lowerLimit, int upperLimit) {

	if (lowerLimit >= upperLimit)
		return lowerLimit;

	return lowerLimit + (rand() % (upperLimit-lowerLimit+1));
}

uint8_t codeDirect(uint8_t b, int k, int s) {
	
	uint8_t temp = (((b>>5)%2) << 1) + b%2;
	uint8_t temp2 = (b & 30) >> 1;
	uint8_t temp3 = affineTransformation(temp2, MATRIX[(s+k)%4], TRANSLATION);
	uint8_t temp4 = inverse(temp3, POLS[(temp + k)%2]);
	uint8_t temp5 = affineTransformation(temp4, MATRIX[(temp+k)%4], TRANSLATION);
	
	return (temp5 + temp)%16;
}

uint8_t affineTransformation(uint8_t b, int *matrix[], int *translation) {
	int bits[BITS_IN_BOX];
	int bits2[BITS_OUT_BOX];
	int i, j;
	uint8_t ret;
	
	/* obtain bitwise expansion of the byte */
	for (i=0; i < BITS_OUT_BOX; ++i) bits[i] = (b >> i)%2;
	
	/* multiplication by the matrix mod 2 */
	for (i=0; i < BITS_OUT_BOX; ++i) {
		bits2[i] = 0;
		for (j=0; j < BITS_OUT_BOX; ++j) bits2[i] += matrix[i][j]*bits[j];
		bits2[i] = bits2[i]%2;
	}
	
	/* addition of the vector mod 2 */
	for (i=0; i < BITS_OUT_BOX; ++i) bits2[i] = (bits2[i]+translation[i])%2;
	
	/* obtain the result as a byte from the array */
	ret = 0;
	for (i=0; i < BITS_OUT_BOX; ++i) ret += bits2[i] << i;
	
	return ret;
}

uint8_t inverse(uint8_t b, int* modCoefficients) {
	struct POLYNOMIAL mod, p, inv;
	uint8_t ret;
	
	if (!b) return 0;
	
	initPol(&mod);
	initPol(&p);
	initPol(&inv);
	
	setPol(&mod, modCoefficients, BITS_OUT_BOX);
	byteToPoly(&p, b);
	
	inversePol(&inv, &p, &mod);
	ret = polyToByte(&inv);
	
	freePol(&mod);
	freePol(&p);
	freePol(&inv);
	
	return ret;
}

void initPol(struct POLYNOMIAL* p) {
	p->degree = -1;
	p->coefficients = NULL;
}

void setPol(struct POLYNOMIAL* p, int* coefs, int n) {
	int i;

	if (p->coefficients) free(p->coefficients);
	p->coefficients = malloc(sizeof(int)*(n+1));
	for (i=0; i <= n; ++i) p->coefficients[i] = coefs[i]%2;
	p->degree = n;
}

void inversePol(struct POLYNOMIAL* dest, struct POLYNOMIAL* s, struct POLYNOMIAL* mod) {
	struct POLYNOMIAL q, r, p1, p2, v1, v2;
	int d1, d2;
	
	initPol(&q);
	initPol(&r);
	initPol(&p1);
	initPol(&p2);
	initPol(&v1);
	initPol(&v2);
	
	d1 = getDegree(mod);
	d2 = getDegree(s);
	
	copyPol(&p1, mod);
	if (d2 >= d1) divPol(&q, &p2, s, mod);
	else copyPol(&p2, s);
	
	clearPol(&v1);
	clearPol(&v2);
	setNthCoefficient(&v2, 0);
	
	while (1) {
		divPol(&q, &r, &p1, &p2);
		copyPol(&p1, &p2);
		copyPol(&p2, &r);
		
		if (getDegree(&r) < 0) break;
		
		mulPol(&r, &q, &v2);
		addPol(&r, &v1, &r);
		copyPol(&v1, &v2);
		copyPol(&v2, &r);
	}
	
	copyPol(dest, &v2);
	
	freePol(&q);
	freePol(&r);
	freePol(&p1);
	freePol(&p2);
	freePol(&v1);
	freePol(&v2);
}

void freePol(struct POLYNOMIAL* p) {
	p->degree = -1;
	free(p->coefficients);
	p->coefficients = NULL;
}

void byteToPoly(struct POLYNOMIAL *p, uint8_t b) {
	int i;
  
	free(p->coefficients);
	p->coefficients = malloc(sizeof(int)*BITS_IN_BYTE);
	p->degree = BITS_IN_BYTE-1;
	for (i=0; i < BITS_IN_BYTE; ++i) p->coefficients[i] = (b >> i)%2;
}

uint8_t polyToByte(struct POLYNOMIAL* p) {
	uint8_t b = 0;
	int i;
	
	for (i=0; i < BITS_IN_BYTE && i <= p->degree; ++i) b += (p->coefficients[i] << i);
	return b;
}

int getDegree(struct POLYNOMIAL* p) {
	int i;
	if (p->degree < 0) return -1;
	for (i=p->degree; i >= 0 && !p->coefficients[i]; i--);
	return i;
}

void copyPol(struct POLYNOMIAL* dest, struct POLYNOMIAL* src) {
	int* aux = NULL;
	int d = src->degree;
	int i;

	aux = malloc(sizeof(int)*(d+1));
	for (i=0; i <= d; ++i) aux[i] = src->coefficients[i];

	if (dest->coefficients) free(dest->coefficients);
	dest->coefficients = aux;
	dest->degree = d;
}

void clearPol(struct POLYNOMIAL* p) {
	int i;
	for (i=0; i <= p->degree; ++i) p->coefficients[i] = 0;
}

void setNthCoefficient(struct POLYNOMIAL* dest, int n) {
	int d = dest->degree, i;
	
	if (n < 0) return;
	
	if (n <= dest->degree) {
		dest->coefficients[n] = 1;
		return;
	}
	
	dest->coefficients = realloc(dest->coefficients, sizeof(int)*(n+1));
	for (i=d+1; i <n; ++i) dest->coefficients[i] = 0;
	dest->coefficients[n] = 1;
	dest->degree = n;
}

void divPol(struct POLYNOMIAL* q, struct POLYNOMIAL* r, struct POLYNOMIAL* div, struct POLYNOMIAL* dvsr) {
	int d1, d2;
	struct POLYNOMIAL aux, qAux, rAux;
	
	initPol(&aux);
	initPol(&qAux);
	initPol(&rAux);
	
	copyPol(&rAux, div);
	clearPol(&qAux);

	for (d1 = getDegree(&rAux), d2 = getDegree(dvsr);  d1 >= d2; d1 = getDegree(&rAux), d2 = getDegree(dvsr)) {
		shiftPol(&aux, dvsr, d1-d2);
		addPol(&rAux, &rAux, &aux);
		setNthCoefficient(&qAux, d1-d2);
	}
	
	copyPol(r, &rAux);
	copyPol(q, &qAux);
	
	freePol(&aux);
	freePol(&qAux);
	freePol(&rAux);
}

void mulPol(struct POLYNOMIAL* dest, struct POLYNOMIAL* s1, struct POLYNOMIAL* s2) {
	int d1 = s1->degree;
	int d2 = s2->degree;
	int d = d1+d2;
	int *aux = NULL;
	int i,j;
	
	if (d1 < 0 || d2 < 0) {
		if (dest->coefficients) free(dest->coefficients);
		dest->coefficients = NULL;
		dest->degree = -1; 
		return;
	}
	
	aux = malloc(sizeof(int)*(d+1));
	for (i=0; i <= d; ++i) {
		aux[i] = 0;
		for (j= (d1 < i)? d1 : i; j >= 0 && (i-j <= d2); j--) aux[i] += s1->coefficients[j]*s2->coefficients[i-j];
		aux[i] = aux[i]%2;
	}
  
	if (dest->coefficients) free(dest->coefficients);
	dest->coefficients = aux;
	dest->degree = d;
}

void addPol(struct POLYNOMIAL* dest, struct POLYNOMIAL* s1, struct POLYNOMIAL* s2) {
	int* aux = NULL;
	int greater = s1->degree > s2->degree;
	int d = greater ? s1->degree : s2->degree;
	int d2 = greater ? s2->degree : s1->degree;
	int i;
	
	/* if both polynomials had degree -1 */
	if (d == -1) {
		if (dest->coefficients) free(dest->coefficients);
		dest->coefficients = NULL;
		dest->degree = -1;
		return;
	}
	
	aux = malloc(sizeof(int)*(d+1));
	for (i=0; i <= d2; ++i) aux[i] = (s1->coefficients[i] + s2->coefficients[i])%2;
	for (i=d2+1; i <= d; ++i) aux[i] = greater ? s1->coefficients[i] : s2->coefficients[i];
  
	if (dest->coefficients) free(dest->coefficients);
	dest->coefficients = aux;
	dest->degree = d;
}

void shiftPol(struct POLYNOMIAL* dest, struct POLYNOMIAL* s, int n) {
	int* aux = NULL;
	int d = s->degree;
	int i;
	
	if (n < 0) return;
	
	aux = malloc(sizeof(int)*(d+n+1));
	for (i=0; i < n; ++i) aux[i] = 0;
	for (i=0; i <= d; ++i) aux[i+n] = s->coefficients[i];
	
	if (dest->coefficients) free(dest->coefficients);
	dest->coefficients = aux;
	dest->degree = d + n;
}
