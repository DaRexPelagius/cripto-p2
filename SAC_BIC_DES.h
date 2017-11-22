#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>


/*--------------------------------------------------------------------------
CONSTANTS
--------------------------------------------------------------------------*/
#define SBOX_OUTPUT_PAIRS 6
#define ERR_N_ARGS -1
#define ERR_FLAGS -2
#define MAX_NAME_LENGTH 256
#define SAC_FLAG 1
#define BIC_FLAG 2
#define DES_BLOCK_SIZE 64
#define NUM_ROUNDS 16
#define BITS_IN_PC1 56
#define BITS_IN_PC2 48
#define BITS_IN_IP 64
#define BITS_IN_E 48
#define BITS_IN_P 32
#define BITS_IN_SWAP 64
#define NUM_S_BOXES 8
#define ROWS_PER_SBOX 4
#define COLUMNS_PER_SBOX 16
#define SBOX_INPUT_BITS 6
#define SBOX_OUTPUT_BITS 4
#define BITS_IN_BOX 6
#define BITS_OUT_BOX 4
#define BITS_IN_BYTE 8

/*--------------------------------------------------------------------------
DATA STRUCTURES
--------------------------------------------------------------------------*/

/* Block in DES algorithm */
typedef struct {
	uint8_t block[DES_BLOCK_SIZE+1];	/* block[0] is never used */
} DESblock;


/* Parameters for the Strict Avalanche Criterion statistics */
typedef struct {
	/* Probability of change in each output bit conditioned to a change in each input bit */
	float condProb[NUM_S_BOXES][SBOX_OUTPUT_BITS][SBOX_INPUT_BITS];
} SAC;

/* Parameters for the Bit Independence Criterion statistics */
typedef struct {
	/* Probability of change in each output bit pair conditioned to a change in each input bit */
	float condProb[NUM_S_BOXES][8*SBOX_OUTPUT_PAIRS][SBOX_INPUT_BITS];
} BIC;

typedef struct POLYNOMIAL {
	int degree;
	int* coefficients;
} strPolynomial;

void printUsage(char* message);
int getArgs(int nArgs, char** args, int* criterionFlag, int* nTests, char* outputFileName, int* flagOutput);
int getCriterionFlag(int nArgs, char** args, int* criterionFlag);
int getInteger(int nArgs, char** args, int* integer, char* flag, int flagLength);
int getString(int nArgs, char** args, char* string, char* flag, int flagLength);
void printBIC(FILE* outputFile, BIC* statistics);
void measureBIC(BIC* statistics, int nTests);
void measureSAC(SAC* statistics, int nTests);
void printSAC(FILE* outputFile, SAC* statistics);
unsigned long int*** allocSACacc();
void freeSACacc(unsigned long int*** SACacc);
unsigned short*** allocSboxes();
void singleSAC(unsigned short*** Sboxes, unsigned long int*** bitChangesAcc);
void freeSboxes(unsigned short*** Sboxes);
unsigned long int*** allocBICacc();
void singleBIC(unsigned short*** Sboxes, unsigned long int*** pairChangesAcc);
void freeBICacc(unsigned long int*** BICacc);
void getSboxes(unsigned short*** Sboxes);
unsigned short*** getNewRandomSboxes();
unsigned short*** getNewSboxes();
void newBlock(DESblock* b, int blockSize);
void copyBlock(DESblock* new, DESblock* old, int length);
void alterSboxInput(DESblock* inputBlock, int bit);
void SBoxGeneral(DESblock* new, DESblock* old, short unsigned int*** Sbox);
int randomInt(int lowerLimit, int upperLimit);
uint8_t codeDirect(uint8_t b, int k, int s);
uint8_t affineTransformation(uint8_t b, int *matrix[], int *translation);
uint8_t inverse(uint8_t b, int* modCoefficients);
void initPol(struct POLYNOMIAL* p);
void setPol(struct POLYNOMIAL* p, int* coefs, int n);
void byteToPoly(struct POLYNOMIAL *p, uint8_t b);
uint8_t polyToByte(struct POLYNOMIAL* p);
int getDegree(struct POLYNOMIAL* p);
void copyPol(struct POLYNOMIAL* dest, struct POLYNOMIAL* src);
void clearPol(struct POLYNOMIAL* p);
void setNthCoefficient(struct POLYNOMIAL* dest, int n);
void divPol(struct POLYNOMIAL* q, struct POLYNOMIAL* r, struct POLYNOMIAL* div, struct POLYNOMIAL* dvsr);
void addPol(struct POLYNOMIAL* dest, struct POLYNOMIAL* s1, struct POLYNOMIAL* s2);
void mulPol(struct POLYNOMIAL* dest, struct POLYNOMIAL* s1, struct POLYNOMIAL* s2);
void shiftPol(struct POLYNOMIAL* dest, struct POLYNOMIAL* s, int n);


