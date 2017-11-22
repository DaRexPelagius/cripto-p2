#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>


/*--------------------------------------------------------------------------
CONSTANTS
--------------------------------------------------------------------------*/
#define ERR_N_ARGS -1
#define ERR_FLAGS -2
#define MAX_TEXT_LENGTH 10000
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
#define SBOX_INPUT_BITS 6
#define SBOX_OUTPUT_BITS 4
#define SBOX_OUTPUT_PAIRS 6
#define BITS_IN_BOX 6
#define BITS_OUT_BOX 4
#define BITS_IN_BYTE 8

/* Parameters for the Non-Linearity statistics, in the range [0,1] (0 if 100% linear, 1 if 100% non-linear) */
typedef struct {
	/* Expectation of linearity of each S-box */
	float expectation[NUM_S_BOXES];
	/* Standard deviation of linearity of each S-box */
	float deviation[NUM_S_BOXES];
	/* Distribution (percentage) of the number of bit coincidences of each S-box */
	float distribution[NUM_S_BOXES*(SBOX_OUTPUT_BITS+1)];
} NL;

typedef struct POLYNOMIAL {
	int degree;
	int* coefficients;
} strPolynomial;

typedef struct {
	uint8_t block[DES_BLOCK_SIZE+1];	/* block[0] is never used */
} DESblock;


void printUsage(char* message);
int getArgs(int nArgs, char** args, int* nTests, char* outputFileName, int* flagOutput);
int getString(int nArgs, char** args, char* string, char* flag, int flagLength);
int getInteger(int nArgs, char** args, int* integer, char* flag, int flagLength);
void printNL(FILE* outputFile, NL* statistics);
void measureNL(NL* statistics, int nTests);
unsigned short*** allocSboxes();
void freeSboxes(unsigned short*** Sboxes);
void singleNL(unsigned short*** Sboxes, unsigned long int* coincCounters);
unsigned short*** getNewRandomSboxes();
unsigned short*** getNewSboxes();
void getSboxes(unsigned short*** Sboxes);
void newBlock(DESblock* b, int blockSize);
void xorDES(DESblock* new, DESblock* old1, DESblock* old2, int length);
void SBoxGeneral(DESblock* new, DESblock* old, short unsigned int*** Sbox);
uint8_t codeDirect(uint8_t b, int k, int s);
uint8_t affineTransformation(uint8_t b, int *matrix[], int *translation);
int randomInt(int lowerLimit, int upperLimit);
uint8_t inverse(uint8_t b, int* modCoefficients);
void initPol(struct POLYNOMIAL* p);
void setPol(struct POLYNOMIAL* p, int* coefs, int n);
void byteToPoly(struct POLYNOMIAL *p, uint8_t b);
void inversePol(struct POLYNOMIAL* dest, struct POLYNOMIAL* s, struct POLYNOMIAL* mod);
uint8_t polyToByte(struct POLYNOMIAL* p);
void freePol(struct POLYNOMIAL* p) ;
int getDegree(struct POLYNOMIAL* p);
void clearPol(struct POLYNOMIAL* p);
void divPol(struct POLYNOMIAL* q, struct POLYNOMIAL* r, struct POLYNOMIAL* div, struct POLYNOMIAL* dvsr);
void copyPol(struct POLYNOMIAL* dest, struct POLYNOMIAL* src);
void setNthCoefficient(struct POLYNOMIAL* dest, int n);
void shiftPol(struct POLYNOMIAL* dest, struct POLYNOMIAL* s, int n);
void addPol(struct POLYNOMIAL* dest, struct POLYNOMIAL* s1, struct POLYNOMIAL* s2);
void mulPol(struct POLYNOMIAL* dest, struct POLYNOMIAL* s1, struct POLYNOMIAL* s2);
