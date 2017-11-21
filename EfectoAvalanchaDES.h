#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>



/*--------------------------------------------------------------------------
CONSTANTS
--------------------------------------------------------------------------*/
#define ERR_N_ARGS -1
#define ERR_FLAGS -2
#define MAX_NAME_LENGTH 256

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


/*--------------------------------------------------------------------------
DATA STRUCTURES
--------------------------------------------------------------------------*/

/* Parameters for the Avalanche Effect statistics */
typedef struct {
	/* Average number of bits that differ in each round when changing one bit of the plaintext */
	float textBitChanges[NUM_ROUNDS+1];
	/* Average number of bits that differ in each round when changing one bit of the key */
	float keyBitChanges[NUM_ROUNDS+1];
} AE;


/* Block in DES algorithm */
typedef struct {
	uint8_t block[DES_BLOCK_SIZE+1];	/* block[0] is never used */
} DESblock;




int getInteger(int nArgs, char** args, int* integer, char* flag, int flagLength);
int getArgs(int nArgs, char** args, int* nTests, char* outputFileName, int* flagOutput);
int getString(int nArgs, char** args, char* string, char* flag, int flagLength);
void measureAE(AE* statistics, int nTests);
void printAE(FILE* outputFile, AE* statistics);
void singleTextAE(DESblock* input, DESblock* key, unsigned long int* bitChangesAcc, short unsigned int*** Sboxes);
void singleKeyAE(DESblock* input, DESblock* key, unsigned long int* bitChangesAcc, short unsigned int*** Sboxes);
void newKey(DESblock* key);
void newBlock(DESblock* b, int blockSize);
void copyBlock(DESblock* new, DESblock* old, int length);
void alterBlock(DESblock* b, int blockSize);
int countDifferences(DESblock* b1, DESblock* b2, int blockSize);
void initialPerm(DESblock* new, DESblock* old);
void leftSemiBlock(DESblock* semiBlock, DESblock* fullBlock);
void rightSemiBlock(DESblock* semiBlock, DESblock* fullBlock);
void permChoice1(DESblock* new, DESblock* old);
void LCS(DESblock* new, DESblock* old, int nRound, int flag);
void permChoice2(DESblock* new, DESblock* old);
void singleRoundGeneral(DESblock* newLeft, DESblock* newRight, DESblock* oldLeft, DESblock* oldRight, DESblock* key, int nRound, short unsigned int*** Sbox);
void selectDES(DESblock* new, DESblock* old, const unsigned short* indices, int length);
void shiftLeftDES(DESblock* new, DESblock* old, int shift);
void shiftRightDES(DESblock* new, DESblock* old, int shift);
void expansion(DESblock* new, DESblock* old);
void xorDES(DESblock* new, DESblock* old1, DESblock* old2, int length);
void SBoxGeneral(DESblock* new, DESblock* old, short unsigned int*** Sbox);
void permutation(DESblock* new, DESblock* old);
int randomInt(int lowerLimit, int upperLimit);
unsigned short*** allocSboxes();
void freeSboxes(unsigned short*** Sboxes);
void printUsage(char* message);
void freeSboxes(unsigned short*** Sboxes);








