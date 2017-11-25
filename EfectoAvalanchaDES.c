#include "EfectoAvalanchaDES.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

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

	int nTests,flagOutput;
	char outputFileName[MAX_NAME_LENGTH];
	FILE* outputFile = NULL;
	AE statistics;

	/* Gets command-line arguments */
	switch (getArgs(argc,argv,&nTests,outputFileName,&flagOutput)) {
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

	/* AE measurement */
	measureAE(&statistics,nTests);
	printAE(outputFile,&statistics);

	if (outputFile && (outputFile != stdout))
		fclose(outputFile);

	return 0;
}


int getArgs(int nArgs, char** args, int* nTests, char* outputFileName, int* flagOutput) {

	if ((nArgs != 3) && (nArgs != 5))
		return ERR_N_ARGS;

	if (getInteger(nArgs,args,nTests,"-t",2) != 1)
		return ERR_FLAGS;

	if (nArgs == 3)
		*flagOutput = 0;
	else if (((*flagOutput) = getString(nArgs,args,outputFileName,"-o",2)) != 1)
		return ERR_FLAGS;

	return 0;
}

/*--------------------------------------------------------------------------
Obtiene un entero
- Entrada:
	* Número de argumentos
	* Array de argumentos
	* Puntero al entero
	* Puntero al modo
	* Longitud de la cadena
- Salida:
	* -1 si ocurre algun error
	* 1 si va bien
--------------------------------------------------------------------------*/
int getEntero(int nArgs, char** args, int* entero, char* modo, int longitud) {

	int i;
	int flag = 0;

	for (i=1; i <= (nArgs - 2); i++){
				
		if ((strncmp(args[i], modo, longitud) == 0) && (strlen(args[i]) == longitud)) {
			if (flag) return -1;
			else {
				*entero = atoi(args[i+1]);
				if ((*entero == 0) && !isdigit(args[i + 1][0])) return -1;
				flag = 1;
			}
		}}

	return flag;
}
/*--------------------------------------------------------------------------
Obtiene una cadena
- Entrada:
	* Número de argumentos
	* Array de argumentos
	* Puntero a la cadena
	* Puntero al modo
	* Longitud de la cadena
- Salida:
	* -1 si ocurre algun error
	* 1 si va bien
--------------------------------------------------------------------------*/
int getCadena(int nArgs, char** args, char* cadena, char* modo, int longitud) {

	int i;
	int flag = 0;

	for (i = 1; i <= (nArgs - 2); i++)
		if ((strncmp(args[i], modo, longitud) == 0) && (strlen(args[i]) == longitud)) {
			if (flag)
				return -1;
			else {
				strcpy(cadena, args[i+1]);
				flag = 1;
			}
		}

	return flag;
}

void measureAE(AE* statistics, int nTests) {

	DESblock key, input;
	int i;
	unsigned long int textBitChangesAcc[NUM_ROUNDS+1];
	unsigned long int keyBitChangesAcc[NUM_ROUNDS+1];
	short unsigned int*** Sboxes = NULL;

	srand(time(NULL));

	/* Initialises the counters */
	for (i=0; i<NUM_ROUNDS+1; i++) {
		textBitChangesAcc[i] = 0;
		keyBitChangesAcc[i] = 0;
	}

	/* Measures text AE and key AE */
	Sboxes = allocSboxes();
	for (i=0; i<nTests; i++) {
		newKey(&key);
		newBloque(&input,DES_BLOCK_SIZE);
		singleTextAE(&input,&key,textBitChangesAcc,Sboxes);
		singleKeyAE(&input,&key,keyBitChangesAcc, Sboxes);
	}
	freeSboxes(Sboxes);

	/* Computes the average */
	for (i=0; i<NUM_ROUNDS+1; i++) {
		statistics->textBitChanges[i] = (textBitChangesAcc[i])*(1./nTests);
		statistics->keyBitChanges[i] = (keyBitChangesAcc[i])*(1./nTests);
	}
}

void printAE(FILE* outputFile, AE* statistics) {

	int i;

	fprintf(outputFile,"Number of bits that differ after a bit change in the plaintext:\n");
	for (i=0; i<NUM_ROUNDS+1; i++)
		fprintf(outputFile,"- Round %d:\t%.2f\n",i,statistics->textBitChanges[i]);

	fprintf(outputFile,"Number of bits that differ after a bit change in the key:\n");
	for (i=0; i<NUM_ROUNDS+1; i++)
		fprintf(outputFile,"- Round %d:\t%.2f\n",i,statistics->keyBitChanges[i]);
}

void singleTextAE(DESblock* input, DESblock* key, unsigned long int* bitChangesAcc, short unsigned int*** Sboxes) {

	DESblock oldLeft, oldRight, newLeft, newRight;
	DESblock ipBlock;
	DESblock oldKey, newKey, roundKey;
	DESblock input2;
	DESblock oldLeft2, oldRight2, newLeft2, newRight2;
	DESblock ipBlock2;
	int i;

	/* Modifies the input */
	copiarBloque(&input2,input,DES_BLOCK_SIZE);
	alterBlock(&input2,DES_BLOCK_SIZE);
	bitChangesAcc[0] += calculaDiferencias(input,&input2,DES_BLOCK_SIZE);

	/* Initial permutation */
	initialPerm(&ipBlock,input);
	initialPerm(&ipBlock2,&input2);

	/* Splits the block into the two halves */
	leftSemiBlock(&oldLeft,&ipBlock);
	rightSemiBlock(&oldRight,&ipBlock);
	leftSemiBlock(&oldLeft2,&ipBlock2);
	rightSemiBlock(&oldRight2,&ipBlock2);

	/* Permutation Choice 1 */
	permChoice1(&oldKey,key);

	/* NUM_ROUNDS rounds */
	for (i=1; i<= NUM_ROUNDS; i++) {

		LCS(&newKey,&oldKey,i,ENCRYPT_FLAG);
		permChoice2(&roundKey,&newKey);

		singleRoundGeneral(&newLeft,&newRight,&oldLeft,&oldRight,&roundKey,i,Sboxes);
		singleRoundGeneral(&newLeft2,&newRight2,&oldLeft2,&oldRight2,&roundKey,i, Sboxes);

		bitChangesAcc[i] += calculaDiferencias(&newLeft,&newLeft2,BITS_IN_FEISTEL/2);
		bitChangesAcc[i] += calculaDiferencias(&newRight,&newRight2,BITS_IN_FEISTEL/2);

		copiarBloque(&oldLeft,&newLeft,BITS_IN_FEISTEL/2);
		copiarBloque(&oldRight,&newRight,BITS_IN_FEISTEL/2);
		copiarBloque(&oldLeft2,&newLeft2,BITS_IN_FEISTEL/2);
		copiarBloque(&oldRight2,&newRight2,BITS_IN_FEISTEL/2);
		copiarBloque(&oldKey,&newKey,BITS_IN_PC1);
	}
}

void singleKeyAE(DESblock* input, DESblock* key, unsigned long int* bitChangesAcc, short unsigned int*** Sboxes) {

	DESblock oldLeft, oldRight, newLeft, newRight;
	DESblock ipBlock;
	DESblock oldKey, newKey, roundKey;
	DESblock key2;
	DESblock oldKey2, newKey2, roundKey2;
	DESblock oldLeft2, oldRight2, newLeft2, newRight2;
	int i;

	/* Modifies the key */
	copiarBloque(&key2,key,DES_KEY_SIZE);
	alterBlock(&key2,DES_KEY_SIZE);

	/* Initial permutation */
	initialPerm(&ipBlock,input);

	/* Splits the block into the two halves */
	leftSemiBlock(&oldLeft,&ipBlock);
	rightSemiBlock(&oldRight,&ipBlock);
	copiarBloque(&oldLeft2,&oldLeft,BITS_IN_FEISTEL/2);
	copiarBloque(&oldRight2,&oldRight,BITS_IN_FEISTEL/2);

	/* Permutation Choice 1 */
	permChoice1(&oldKey,key);
	permChoice1(&oldKey2,&key2);

	/* NUM_ROUNDS rounds */
	for (i=1; i<= NUM_ROUNDS; i++) {

		LCS(&newKey,&oldKey,i,ENCRYPT_FLAG);
		LCS(&newKey2,&oldKey2,i,ENCRYPT_FLAG);
		permChoice2(&roundKey,&newKey);
		permChoice2(&roundKey2,&newKey2);

		singleRoundGeneral(&newLeft,&newRight,&oldLeft,&oldRight,&roundKey,i,Sboxes);
		singleRoundGeneral(&newLeft2,&newRight2,&oldLeft2,&oldRight2,&roundKey2,i,Sboxes);

		bitChangesAcc[i] += calculaDiferencias(&newLeft,&newLeft2,BITS_IN_FEISTEL/2);
		bitChangesAcc[i] += calculaDiferencias(&newRight,&newRight2,BITS_IN_FEISTEL/2);

		copiarBloque(&oldLeft,&newLeft,BITS_IN_FEISTEL/2);
		copiarBloque(&oldRight,&newRight,BITS_IN_FEISTEL/2);
		copiarBloque(&oldLeft2,&newLeft2,BITS_IN_FEISTEL/2);
		copiarBloque(&oldRight2,&newRight2,BITS_IN_FEISTEL/2);
		copiarBloque(&oldKey,&newKey,BITS_IN_PC1);
		copiarBloque(&oldKey2,&newKey2,BITS_IN_PC1);
	}
}

void newClave(Bloque* clave) {

	int byte, bit;
	int acc;

	for (byte = 0; byte < (TAM_CLAVE / 8); byte++) {
		acc = 0;
		for (bit = 0; bit < 7; bit++) acc += (clave->bloque[8 * byte + bit + 1] = naleatorio(0,1));
		clave->bloque[8 * byte + 7 + 1] = (acc % 2 == 0);
	}
}

void newBloque(DESblock* b, int tamBloque) {

	int i;

	for (i = 1; i <= tamBloque; i++) b->block[i] = naleatorio(0,1);
}

void copiarBloque(DESblock* new, DESblock* old, int length) {

	int i;

	for (i = 1; i <= length; i++) new->block[i] = old->block[i];
}

void alterBlock(DESblock* b, int tamBloque) {

	int i = naleatorio(1,tamBloque);

	if (b->block[i] == 0) b->block[i] = 1;
	else b->block[i] = 0;
}

int calculaDiferencias(DESblock* b1, DESblock* b2, int tamBloque) {

	int i;
	int n = 0;

	for (i=1; i<= tamBloque; i++) if (b1->block[i] != b2->block[i]) n++;

	return n;
}

void initialPerm(DESblock* new, DESblock* old) {
	selectDES(new, old, IP, BITS_IN_IP);
}

void leftSemiBlock(DESblock* semiBlock, DESblock* fullBlock) {

	int i;

	for (i=1; i <= BITS_IN_FEISTEL/2; i++)
		semiBlock->block[i] = fullBlock->block[i];
}


void rightSemiBlock(DESblock* semiBlock, DESblock* fullBlock) {

	int i;

	for (i=1; i <= BITS_IN_FEISTEL/2; i++)
		semiBlock->block[i] = fullBlock->block[i+BITS_IN_FEISTEL/2];
}

void permChoice1(DESblock* new, DESblock* old) {
	selectDES(new, old, PC1, BITS_IN_PC1);
}

void LCS(DESblock* new, DESblock* old, int nRound, int flag) {

	if (flag == ENCRYPT_FLAG)
		shiftLeftDES(new, old, ROUND_SHIFTS[nRound-1]);
	else if (flag == DECRYPT_FLAG)
		shiftRightDES(new, old, ROUND_SHIFTS_DEC[nRound-1]);
}

void permChoice2(DESblock* new, DESblock* old) {
	selectDES(new, old, PC2, BITS_IN_PC2);
}

void singleRoundGeneral(DESblock* newLeft, DESblock* newRight, DESblock* oldLeft, DESblock* oldRight, DESblock* key, int nRound, short unsigned int*** Sbox) {
	DESblock eBlock, xBlock, sBlock, pBlock;

	/* F function */
	expansion(&eBlock,oldRight);
	xorDES(&xBlock,&eBlock,key,BITS_IN_E);
	SBoxGeneral(&sBlock,&xBlock, Sbox);
	permutation(&pBlock,&sBlock);

	/* L(i) = R(i-1) */
	copiarBloque(newLeft,oldRight,BITS_IN_FEISTEL/2);

	/* R(i) = L(i-1) xor F(R(i-1),k(i)) */
	xorDES(newRight,oldLeft,&pBlock,BITS_IN_P);
}

void selectDES(DESblock* new, DESblock* old, const unsigned short* indices, int length) {

	int i;

	for (i=1; i <= length; i++)
		new->block[i] = old->block[indices[i-1]];
}

void shiftLeftDES(DESblock* new, DESblock* old, int shift) {

	int i;

	for (i=0; i < BITS_IN_HALFBLOCK; i++)
		new->block[i+1] = old->block[((i+shift)%(BITS_IN_HALFBLOCK)) + 1];
	for (i=0; i < BITS_IN_HALFBLOCK; i++)
		new->block[i+BITS_IN_HALFBLOCK+1] = old->block[((i+shift)%(BITS_IN_HALFBLOCK)) + BITS_IN_HALFBLOCK + 1];
}

void shiftRightDES(DESblock* new, DESblock* old, int shift) {

	int i;

	for (i=0; i < BITS_IN_HALFBLOCK; i++)
		new->block[i+1] = old->block[((i-shift+BITS_IN_HALFBLOCK)%(BITS_IN_HALFBLOCK)) + 1];
	for (i=0; i < BITS_IN_HALFBLOCK; i++)
		new->block[i+BITS_IN_HALFBLOCK+1] = old->block[((i-shift+BITS_IN_HALFBLOCK)%(BITS_IN_HALFBLOCK)) + BITS_IN_HALFBLOCK + 1];
}

void expansion(DESblock* new, DESblock* old) {
	selectDES(new, old, E, BITS_IN_E);
}

void xorDES(DESblock* new, DESblock* old1, DESblock* old2, int length) {

	int i;

	for (i=1; i <= length; i++) 
		new->block[i] = (old1->block[i] != old2->block[i]);
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

void permutation(DESblock* new, DESblock* old) {
	selectDES(new, old, P, BITS_IN_P);
}

int naleatorio(int a, int b) {

	if (a >= b) return a;

	return a + (rand() % (b - a + 1));
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

void getSboxes(unsigned short*** Sboxes) {

	int box, r, c;

	for (box=0; box<NUM_S_BOXES; box++)
		for (r=0; r<ROWS_PER_SBOX; r++)
			for (c=0; c<COLUMNS_PER_SBOX; c++)
				Sboxes[box][r][c] = S_BOXES[box][r][c];
}

void printUsage(char* message) {

	if (message)
		fprintf(stderr,"%s",message);

	fprintf(stderr,"Syntax: ./desAE {-t nTests} [-o outputFile]\n");
	fprintf(stderr,"- nTests:\tNumber of tests to perform in order to measure the Avalanche Effect\n");
	fprintf(stderr,"- outputFile:\tOutput file (default: stdout)\n");
}


