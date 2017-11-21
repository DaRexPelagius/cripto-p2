#ifndef __DES_H
#define __DES_H


/*--------------------------------------------------------------------------
LIBRARIES
--------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>


/*--------------------------------------------------------------------------
CONSTANTS
--------------------------------------------------------------------------*/
#define ENCRYPT_FLAG 1
#define DECRYPT_FLAG 2
#define DES_BLOCK_SIZE 64
#define DES_KEY_SIZE 64
#define NUM_ROUNDS 16
#define BITS_IN_FEISTEL 64
#define BITS_IN_KEY 56
#define BITS_IN_PC1 56
#define BITS_IN_PC2 48
#define BITS_IN_HALFBLOCK BITS_IN_KEY/2
#define BITS_IN_IP 64
#define BITS_IN_E 48
#define BITS_IN_P 32
#define BITS_IN_SWAP 64
#define NUM_S_BOXES 8
#define ROWS_PER_SBOX 4
#define COLUMNS_PER_SBOX 16
#define SBOX_INPUT_BITS 6
#define SBOX_OUTPUT_BITS 4


/*--------------------------------------------------------------------------
DATA STRUCTURES
--------------------------------------------------------------------------*/

/* Block in DES algorithm */
typedef struct {
	uint8_t block[DES_BLOCK_SIZE+1];	/* block[0] is never used */
} DESblock;


/*--------------------------------------------------------------------------
PUBLIC FUNCTION DEFINITIONS
--------------------------------------------------------------------------*/
void leftSemiBlock(DESblock* semiBlock, DESblock* fullBlock);
void rightSemiBlock(DESblock* semiBlock, DESblock* fullBlock);
void mergeSemiBlocks(DESblock* fullBlock, DESblock* left, DESblock* right);
void copyBlock(DESblock* new, DESblock* old, int length);
void selectDES(DESblock* new, DESblock* old, const unsigned short* indices, int length);
void shiftLeftDES(DESblock* new, DESblock* old, int shift);
void shiftRightDES(DESblock* new, DESblock* old, int shift);
void xorDES(DESblock* new, DESblock* old1, DESblock* old2, int length);
void permChoice1(DESblock* new, DESblock* old);
void permChoice2(DESblock* new, DESblock* old);
void LCS(DESblock* new, DESblock* old, int nRound, int flag);
void SBox(DESblock* new, DESblock* old);
void expansion(DESblock* new, DESblock* old);
void permutation(DESblock* new, DESblock* old);
void initialPerm(DESblock* new, DESblock* old);
void invInitialPerm(DESblock* new, DESblock* old);
void swap(DESblock* new, DESblock* old);
void singleRound(DESblock* newLeft, DESblock* newRight, DESblock* oldLeft, DESblock* oldRight, DESblock* key, int nRound);
void singleRoundGeneral(DESblock* newLeft, DESblock* newRight, DESblock* oldLeft, DESblock* oldRight, DESblock* key, int nRound, short unsigned int*** Sbox);


void DES(DESblock* output, DESblock* input, DESblock* key, int flag);

void getSboxes(unsigned short*** Sboxes);
void SBoxGeneral(DESblock* new, DESblock* old, short unsigned int*** Sbox);

void printBinBlock(FILE* outputFile, DESblock* b, int blockSize, char* text);
void printHexaBlock(FILE* outputFile, DESblock* b, int blockSize, char* text);

void newKey(DESblock* key);
int validKey(DESblock* key);
void printKey(FILE* outputFile, DESblock* key);

void textToBlock(DESblock* b, char* text, int blockSize);
void blockToText(char* text, DESblock* b, int blockSize);
int hexaToBlock(DESblock* newBlock, char* string, int length);

#endif

