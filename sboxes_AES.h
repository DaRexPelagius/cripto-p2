#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/*--------------------------------------------------------------------------
CONSTANTS
--------------------------------------------------------------------------*/
#define ERR_N_ARGS -1
#define ERR_FLAGS -2
#define MAX_NAME_LENGTH 256
#define DECRYPT_FLAG 0
#define ENCRYPT_FLAG 1
#define BOX_SIZE 256
#define BITS_IN_BYTE 8

/*--------------------------------------------------------------------------
DATA STRUCTURES
--------------------------------------------------------------------------*/


typedef struct POLYNOMIAL {
	int degree;
	int* coefficients;
} strPolynomial;


typedef struct AESBOX {
	uint8_t map[BOX_SIZE];
} strAesBox;


void printUsage(char* message);
int getArgs(int nArgs, char** args, int* flag, char* outputFileName, int* flagOutput);
int getFlag(int nArgs, char** args, int* flag);
int getString(int nArgs, char** args, char* string, char* flag, int flagLength);
void calculateDirectBox(struct AESBOX* box);
void calculateInverseBox(struct AESBOX* box);
void writeBox(FILE* f, struct AESBOX* box);
uint8_t codeDirect(uint8_t b);
uint8_t codeInverse(uint8_t b);
uint8_t affineDirect(uint8_t b);
uint8_t affineInverse(uint8_t b);
uint8_t inverse(uint8_t b);
uint8_t affineTransformation(uint8_t b, int *matrix[], int *translation);
void initPol(struct POLYNOMIAL* p);
void byteToPoly(struct POLYNOMIAL *p, uint8_t b);
void setPol(struct POLYNOMIAL* p, int* coefs, int n);
void inversePol(struct POLYNOMIAL* dest, struct POLYNOMIAL* s, struct POLYNOMIAL* mod);
uint8_t polyToByte(struct POLYNOMIAL* p);
void freePol(struct POLYNOMIAL* p);
int getDegree(struct POLYNOMIAL* p);
void copyPol(struct POLYNOMIAL* dest, struct POLYNOMIAL* src);
void divPol(struct POLYNOMIAL* q, struct POLYNOMIAL* r, struct POLYNOMIAL* div, struct POLYNOMIAL* dvsr);
void clearPol(struct POLYNOMIAL* p);
void setNthCoefficient(struct POLYNOMIAL* dest, int n);
void mulPol(struct POLYNOMIAL* dest, struct POLYNOMIAL* s1, struct POLYNOMIAL* s2);
void addPol(struct POLYNOMIAL* dest, struct POLYNOMIAL* s1, struct POLYNOMIAL* s2);
void shiftPol(struct POLYNOMIAL* dest, struct POLYNOMIAL* s, int n);
