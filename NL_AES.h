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
#define BUFFER_SIZE 100
#define SIZE_OF_CHAR 8
#define MAX_TEXT_LENGTH 10000
#define MAX_NAME_LENGTH 256
#define DECRYPT_FLAG 0
#define ENCRYPT_FLAG 1
#define SAC_FLAG 1
#define BIC_FLAG 2
#define SBOX_OUTPUT_BITS 8
#define SBOX_INPUT_BITS 8
#define BOX_SIZE 256
#define BITS_IN_BYTE 8
#define BLOCK_WIDTH 4
#define BYTES_IN_WORD 4
#define AES_ROUNDS 10

/*--------------------------------------------------------------------------
DATA STRUCTURES
--------------------------------------------------------------------------*/


typedef struct AESBLOCK {
	uint8_t block[BLOCK_WIDTH][BYTES_IN_WORD];
} strAesBlock;

/* Parameters for the Non-Linearity statistics, in the range [0,1] (0 if 100% linear, 1 if 100% non-linear) */
typedef struct {
	/* Expectation of linearity */
	float expectation;
	/* Standard deviation of linearity */
	float deviation;
	/* Distribution (percentage) of the number of bit coincidences */
	float distribution[SBOX_OUTPUT_BITS+1];
} NL;

typedef struct AESEXP {
	uint8_t map[(AES_ROUNDS+1)*BLOCK_WIDTH][BYTES_IN_WORD];
} strAesExp;

typedef struct AESBOX {
	uint8_t map[BOX_SIZE];
} strAesBox;

typedef struct POLYNOMIAL {
	int degree;
	int* coefficients;
} strPolynomial;


int getArgs(int nArgs, char** args, int* nTests, char* outputFileName, int* flagOutput);
void printUsage(char* message);
int getInteger(int nArgs, char** args, int* integer, char* flag, int flagLength);
int getString(int nArgs, char** args, char* string, char* flag, int flagLength);
void printNL(FILE* outputFile, NL* statistics);
void measureNL(NL* statistics, int nTests);
void calculateDirectBox(struct AESBOX* box);
uint8_t codeDirect(uint8_t b);
uint8_t affineDirect(uint8_t b);
uint8_t affineTransformation(uint8_t b, int *matrix[], int *translation);
uint8_t inverse(uint8_t b);
int singleNL(struct AESBOX* box);
void initPol(struct POLYNOMIAL* p);
void setPol(struct POLYNOMIAL* p, int* coefs, int n);
void byteToPoly(struct POLYNOMIAL *p, uint8_t b);
void inversePol(struct POLYNOMIAL* dest, struct POLYNOMIAL* s, struct POLYNOMIAL* mod);
uint8_t polyToByte(struct POLYNOMIAL* p);
void freePol(struct POLYNOMIAL* p);
int getDegree(struct POLYNOMIAL* p);
void addPol(struct POLYNOMIAL* dest, struct POLYNOMIAL* s1, struct POLYNOMIAL* s2);
void mulPol(struct POLYNOMIAL* dest, struct POLYNOMIAL* s1, struct POLYNOMIAL* s2);
void copyPol(struct POLYNOMIAL* dest, struct POLYNOMIAL* src);
void divPol(struct POLYNOMIAL* q, struct POLYNOMIAL* r, struct POLYNOMIAL* div, struct POLYNOMIAL* dvsr);
void clearPol(struct POLYNOMIAL* p);
void setNthCoefficient(struct POLYNOMIAL* dest, int n);
void shiftPol(struct POLYNOMIAL* dest, struct POLYNOMIAL* s, int n);
