/*--------------------------------------------------------------------------
LIBRARIES
--------------------------------------------------------------------------*/
#include "DES.h"
#include "number.h"
#include <string.h>
#include <stdio.h>


/*--------------------------------------------------------------------------
PARAMETERS
--------------------------------------------------------------------------*/

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


/*--------------------------------------------------------------------------
FUNCTIONS
--------------------------------------------------------------------------*/

/*··········································································
INTERNAL DES FUNCTIONS
··········································································*/

/*--------------------------------------------------------------------------
Gets the left half of a full block
- Input:
	+ Left half of the block
	+ Full block
- Output: Void
--------------------------------------------------------------------------*/
void leftSemiBlock(DESblock* semiBlock, DESblock* fullBlock) {

	int i;

	for (i=1; i <= BITS_IN_FEISTEL/2; i++)
		semiBlock->block[i] = fullBlock->block[i];
}


/*--------------------------------------------------------------------------
Gets the right half of a full block
- Input:
	+ Right half of the block
	+ Full block
- Output: Void
--------------------------------------------------------------------------*/
void rightSemiBlock(DESblock* semiBlock, DESblock* fullBlock) {

	int i;

	for (i=1; i <= BITS_IN_FEISTEL/2; i++)
		semiBlock->block[i] = fullBlock->block[i+BITS_IN_FEISTEL/2];
}


/*--------------------------------------------------------------------------
Merges the two halves of a block
- Input:
	+ Right half of the block
	+ Full block
- Output: Void
--------------------------------------------------------------------------*/
void mergeSemiBlocks(DESblock* fullBlock, DESblock* left, DESblock* right) {

	int i;

	for (i=1; i <= BITS_IN_FEISTEL/2; i++) {
		fullBlock->block[i] = left->block[i];
		fullBlock->block[BITS_IN_FEISTEL/2+i] = right->block[i];
	}
}


/*--------------------------------------------------------------------------
Copies a block from another one
- Input:
	+ New block
	+ Previous block
	+ Block length
- Output: Void
--------------------------------------------------------------------------*/
void copyBlock(DESblock* new, DESblock* old, int length) {

	int i;

	for (i=1; i <= length; i++)
		new->block[i] = old->block[i];
}


/*--------------------------------------------------------------------------
Permutates a block according to the given indices
- Input:
	+ New block
	+ Previous block
	+ Indices of the permutation
	+ Block length
- Output: Void
--------------------------------------------------------------------------*/
void selectDES(DESblock* new, DESblock* old, const unsigned short* indices, int length) {

	int i;

	for (i=1; i <= length; i++)
		new->block[i] = old->block[indices[i-1]];
}


/*--------------------------------------------------------------------------
Shifts left the two halves of a block according to the given shift
- Input:
	+ New block
	+ Previous block
	+ Shift
- Output: Void
--------------------------------------------------------------------------*/
void shiftLeftDES(DESblock* new, DESblock* old, int shift) {

	int i;

	for (i=0; i < BITS_IN_HALFBLOCK; i++)
		new->block[i+1] = old->block[((i+shift)%(BITS_IN_HALFBLOCK)) + 1];
	for (i=0; i < BITS_IN_HALFBLOCK; i++)
		new->block[i+BITS_IN_HALFBLOCK+1] = old->block[((i+shift)%(BITS_IN_HALFBLOCK)) + BITS_IN_HALFBLOCK + 1];
}


/*--------------------------------------------------------------------------
Shifts right the two halves of a block according to the given shift
- Input:
	+ New block
	+ Previous block
	+ Shift
- Output: Void
--------------------------------------------------------------------------*/
void shiftRightDES(DESblock* new, DESblock* old, int shift) {

	int i;

	for (i=0; i < BITS_IN_HALFBLOCK; i++)
		new->block[i+1] = old->block[((i-shift+BITS_IN_HALFBLOCK)%(BITS_IN_HALFBLOCK)) + 1];
	for (i=0; i < BITS_IN_HALFBLOCK; i++)
		new->block[i+BITS_IN_HALFBLOCK+1] = old->block[((i-shift+BITS_IN_HALFBLOCK)%(BITS_IN_HALFBLOCK)) + BITS_IN_HALFBLOCK + 1];
}


/*--------------------------------------------------------------------------
XORes a block according to the given indices
- Input:
	+ New block
	+ Previous blocks
	+ Block length
- Output: Void
--------------------------------------------------------------------------*/
void xorDES(DESblock* new, DESblock* old1, DESblock* old2, int length) {

	int i;

	for (i=1; i <= length; i++) 
		new->block[i] = (old1->block[i] != old2->block[i]);
}


/*--------------------------------------------------------------------------
Permutation choice 1 in the DES algorithm
- Input:
	+ New block
	+ Previous block
- Output: Void
--------------------------------------------------------------------------*/
void permChoice1(DESblock* new, DESblock* old) {
	selectDES(new, old, PC1, BITS_IN_PC1);
}


/*--------------------------------------------------------------------------
Permutation choice 2 in the DES algorithm
- Input:
	+ New block
	+ Previous block
- Output: Void
--------------------------------------------------------------------------*/
void permChoice2(DESblock* new, DESblock* old) {
	selectDES(new, old, PC2, BITS_IN_PC2);
}


/*--------------------------------------------------------------------------
Left Circular Shift in the DES algorithm
- Input:
	+ New block
	+ Previous block
	+ Round
	+ Flag for encryption (ENCRYPT_FLAG) / decryption (DECRYPT_FLAG)
- Output: Void
--------------------------------------------------------------------------*/
void LCS(DESblock* new, DESblock* old, int nRound, int flag) {

	if (flag == ENCRYPT_FLAG)
		shiftLeftDES(new, old, ROUND_SHIFTS[nRound-1]);
	else if (flag == DECRYPT_FLAG)
		shiftRightDES(new, old, ROUND_SHIFTS_DEC[nRound-1]);
}


/*--------------------------------------------------------------------------
S-boxes in the DES algorithm
- Input:
	+ New block
	+ Previous block
- Output: Void
--------------------------------------------------------------------------*/
void SBox(DESblock* new, DESblock* old) {

	int i, row, col;
	int value;

	for (i=0; i < NUM_S_BOXES; i++) {
		row = 2*old->block[1+6*i] + old->block[1+6*i+5];
		col = 8*old->block[1+6*i+1] + 4*old->block[1+6*i+2] + 2*old->block[1+6*i+3] + old->block[1+6*i+4];
		
		value = S_BOXES[i][row][col];
		new->block[1+i*4+3] = value%2;
		new->block[1+i*4+2] = (value/2)%2;
		new->block[1+i*4+1] = (value/4)%2;
		new->block[1+i*4] = (value/8)%2;
	}
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


/*--------------------------------------------------------------------------
Expansion E in the DES algorithm
- Input:
	+ New block
	+ Previous block
- Output: Void
--------------------------------------------------------------------------*/
void expansion(DESblock* new, DESblock* old) {
	selectDES(new, old, E, BITS_IN_E);
}


/*--------------------------------------------------------------------------
Permutation P in the DES algorithm
- Input:
	+ New block
	+ Previous block
- Output: Void
--------------------------------------------------------------------------*/
void permutation(DESblock* new, DESblock* old) {
	selectDES(new, old, P, BITS_IN_P);
}


/*--------------------------------------------------------------------------
Initial Permutation in the DES algorithm
- Input:
	+ New block
	+ Previous block
- Output: Void
--------------------------------------------------------------------------*/
void initialPerm(DESblock* new, DESblock* old) {
	selectDES(new, old, IP, BITS_IN_IP);
}


/*--------------------------------------------------------------------------
Inverse of the Initial Permutation in the DES algorithm
- Input:
	+ New block
	+ Previous block
- Output: Void
--------------------------------------------------------------------------*/
void invInitialPerm(DESblock* new, DESblock* old) {
	selectDES(new, old, IP_INV, BITS_IN_IP);
}


/*--------------------------------------------------------------------------
32-bit swap in the DES algorithm
- Input:
	+ New block
	+ Previous block
- Output: Void
--------------------------------------------------------------------------*/
void swap(DESblock* new, DESblock* old) {
	selectDES(new, old, SWAP, BITS_IN_SWAP);
}


/*--------------------------------------------------------------------------
Single round in the DES algorithm
- Input:
	+ New left and right semi-blocks
	+ Previous left and right semi-blocks
	+ Round key
	+ Number of round
- Output: Void
--------------------------------------------------------------------------*/
void singleRound(DESblock* newLeft, DESblock* newRight, DESblock* oldLeft, DESblock* oldRight, DESblock* key, int nRound) {

	DESblock eBlock, xBlock, sBlock, pBlock;

	/* F function */
	expansion(&eBlock,oldRight);
	xorDES(&xBlock,&eBlock,key,BITS_IN_E);
	SBox(&sBlock,&xBlock);
	permutation(&pBlock,&sBlock);

	/* L(i) = R(i-1) */
	copyBlock(newLeft,oldRight,BITS_IN_FEISTEL/2);

	/* R(i) = L(i-1) xor F(R(i-1),k(i)) */
	xorDES(newRight,oldLeft,&pBlock,BITS_IN_P);
}

void singleRoundGeneral(DESblock* newLeft, DESblock* newRight, DESblock* oldLeft, DESblock* oldRight, DESblock* key, int nRound, short unsigned int*** Sbox) {
	DESblock eBlock, xBlock, sBlock, pBlock;

	/* F function */
	expansion(&eBlock,oldRight);
	xorDES(&xBlock,&eBlock,key,BITS_IN_E);
	SBoxGeneral(&sBlock,&xBlock, Sbox);
	permutation(&pBlock,&sBlock);

	/* L(i) = R(i-1) */
	copyBlock(newLeft,oldRight,BITS_IN_FEISTEL/2);

	/* R(i) = L(i-1) xor F(R(i-1),k(i)) */
	xorDES(newRight,oldLeft,&pBlock,BITS_IN_P);
}

/*··········································································
DES
··········································································*/

/*--------------------------------------------------------------------------
DES algorithm for a single block
- Input:
	+ Output block
	+ Input block
	+ Key
	+ Flag for encryption (ENCRYPT_FLAG) / decryption (DECRYPT_FLAG)
- Output: Void
--------------------------------------------------------------------------*/
void DES(DESblock* output, DESblock* input, DESblock* key, int flag) {

	DESblock oldLeft, oldRight, newLeft, newRight;
	DESblock ipBlock, rBlock, sBlock;
	DESblock oldKey, newKey, roundKey;
	int i;

	/* Initial permutation */
	initialPerm(&ipBlock,input);

	/* Splits the block into the two halves */
	leftSemiBlock(&oldLeft,&ipBlock);
	rightSemiBlock(&oldRight,&ipBlock);

	/* Permutation Choice 1 */
	permChoice1(&oldKey,key);

	/* NUM_ROUNDS rounds */
	for (i=1; i<= NUM_ROUNDS; i++) {

		LCS(&newKey,&oldKey,i,flag);
		permChoice2(&roundKey,&newKey);

		singleRound(&newLeft,&newRight,&oldLeft,&oldRight,&roundKey,i);

		copyBlock(&oldLeft,&newLeft,BITS_IN_FEISTEL/2);
		copyBlock(&oldRight,&newRight,BITS_IN_FEISTEL/2);
		copyBlock(&oldKey,&newKey,BITS_IN_PC1);
	}

	/* Merges the semiblocks */
	mergeSemiBlocks(&rBlock,&oldLeft,&oldRight);

	/* 32-bit swap */
	swap(&sBlock,&rBlock);

	/* Inverse permutation */
	invInitialPerm(output,&sBlock);
}


/*··········································································
S-BOXES
··········································································*/

/*--------------------------------------------------------------------------
Obtains DES S-boxes
- Input: S-boxes array
- Output: Void
--------------------------------------------------------------------------*/
void getSboxes(unsigned short*** Sboxes) {

	int box, r, c;

	for (box=0; box<NUM_S_BOXES; box++)
		for (r=0; r<ROWS_PER_SBOX; r++)
			for (c=0; c<COLUMNS_PER_SBOX; c++)
				Sboxes[box][r][c] = S_BOXES[box][r][c];
}


/*··········································································
OUTPUT FUNCTIONS
··········································································*/

/*--------------------------------------------------------------------------
Writes in binary format a DES block
- Input:
	+ Pointer to the file stream in which to write the block
	+ DES block
	+ Block size
	+ Text to display before the block (NULL if none)
- Output: Void
--------------------------------------------------------------------------*/
void printBinBlock(FILE* outputFile, DESblock* b, int blockSize, char* text) {

	int i;

	if (text)
		fprintf(outputFile,"%s",text);

	for (i=1; i<=blockSize; i++)
		fprintf(outputFile,"%d",b->block[i]);
	fputc('\n',outputFile);
}


/*--------------------------------------------------------------------------
Writes in hexadecimal format a DES block of length multiple of 4
- Input:
	+ Pointer to the file stream in which to write the block
	+ DES block
	+ Block size (multiple of 4)
	+ Text to display before the block (NULL if none)
- Output: Void
--------------------------------------------------------------------------*/
void printHexaBlock(FILE* outputFile, DESblock* b, int blockSize, char* text) {

	char c;
	int i, j;
	uint8_t binBuffer[4];

	if (text)
		fprintf(outputFile,"%s",text);
	fprintf(outputFile,"%s","0x");

	for (i=0; i<blockSize/4; i++) {
		for (j=0; j<4; j++)
			binBuffer[j] = b->block[i*4+j+1];
		binToHex(&c,binBuffer);
		fputc(c,outputFile);
	}
	fputc('\n',outputFile);
}


/*··········································································
KEY FUNCTIONS
··········································································*/

/*--------------------------------------------------------------------------
Generates a valid DES key (with odd parity in each byte)
- Input: Pointer to the key
- Output: Void
--------------------------------------------------------------------------*/
void newKey(DESblock* key) {

	int byte, bit;
	int acc;

	for (byte=0; byte<DES_KEY_SIZE/8; byte++) {
		acc = 0;
		for (bit=0; bit<7; bit++)
			acc += (key->block[8*byte+bit+1] = randomInt(0,1));
		key->block[8*byte+7+1] = (acc%2 == 0);
	}
}


/*--------------------------------------------------------------------------
Validates a DES key, checking the parity bits (odd parity)
- Input: Pointer to the key
- Output:
	+ 1 if the key is valid
	+ 0 otherwise
--------------------------------------------------------------------------*/
int validKey(DESblock* key) {

	int byte, bit;
	int acc;

	for (byte=0; byte<DES_KEY_SIZE/8; byte++) {
		acc = 0;
		for (bit=0; bit<8; bit++)
			acc += key->block[8*byte+bit+1];
		if (acc%2 == 0)
			return 0;
	}

	return 1;
}


/*--------------------------------------------------------------------------
Writes the DES key in hexadecimal format
- Input:
	+ Pointer to the file stream in which to write the text
	+ Key block
- Output: Void
--------------------------------------------------------------------------*/
void printKey(FILE* outputFile, DESblock* key) {
	printHexaBlock(outputFile,key,DES_KEY_SIZE,"Key = ");
}


/*··········································································
BLOCK FUNCTIONS
··········································································*/

/*--------------------------------------------------------------------------
Get a binary DES block from character text with enough characters
(blockSize/8)
- Input:
	+ DES block
	+ Character text
	+ Block size (multiple of 8)
- Output: Void
--------------------------------------------------------------------------*/
void textToBlock(DESblock* b, char* text, int blockSize) {

	int i;
	int nBytes = blockSize/8;

	for (i=0; i<nBytes ; i++)
		charToBin(b->block + i*8 + 1, text[i]);
}


/*--------------------------------------------------------------------------
Gets the character text from binary DES block
- Input:
	+ Pointer to the characters array
	+ Blocks to be translated
	+ Block size (multiple of 8)
- Output: Void
--------------------------------------------------------------------------*/
void blockToText(char* text, DESblock* b, int blockSize) {

	int i;
	int nBytes = blockSize/8;

	for (i=0; i<nBytes; i++)
		binToChar(((uint8_t*)text)+i,b->block + 8*i + 1);
}


/*--------------------------------------------------------------------------
Given a string with a DES algorithm block in hexadecimal format (starting
with 0x), gets the corresponding binary block
- Input:
	+ DES block
	+ String with the block
	+ Length of the block (multiple of 4)
- Output:
	+ 0 if there is no error
	+ -1 otherwise
--------------------------------------------------------------------------*/
int hexaToBlock(DESblock* newBlock, char* string, int length) {

	int i;
	
	if (strlen(string) != (length/4 + 2))
		return -1;

	if ((string[0] != '0') || (string[1] != 'x'))
		return -1;

	for (i=0; i<length/4; i++) {
		if (!isHex(string[i+2]))
			return -1;
		hexToBin(&(newBlock->block[4*i+1]),string[i+2]);
	}

	return 0;
}

