#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <ctype.h>



/*--------------------------------------------------------------------------
CONSTANTS
--------------------------------------------------------------------------*/
#define ERR_N_ARGS -1
#define ERR_FLAGS -2
#define MAX_NAME_LENGTH 256

#define ENCRYPT_FLAG 1
#define DECRYPT_FLAG 2
#define TAM_BLOQUE 64
#define TAM_CLAVE 64
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
	uint8_t bloque[TAM_BLOQUE + 1];	/* block[0] is never used */
} BloqueDES;




int getArgs(int nArgs, char** args, int* nTests, char* outputFileName, int* flagOutput);
int getEntero(int nArgs, char** args, int* entero, char* modo, int longitud);
int getCadena(int nArgs, char** args, char* cadena, char* modo, int longitud);
void measureAE(AE* statistics, int nTests);
void printAE(FILE* outputFile, AE* statistics);
void singleTextAE(BloqueDES* input, BloqueDES* key, unsigned long int* bitChangesAcc, short unsigned int*** Sboxes);
void singleKeyAE(BloqueDES* input, BloqueDES* key, unsigned long int* bitChangesAcc, short unsigned int*** Sboxes);
void newClave(BloqueDES* clave);
void newBloque(BloqueDES* b, int tamBloque);
void copiarBloque(BloqueDES* new, BloqueDES* old, int length);
void alterBlock(BloqueDES* b, int tamBloque);
int calculaDiferencias(BloqueDES* b1, BloqueDES* b2, int tamBloque);
void initialPerm(BloqueDES* new, BloqueDES* old);
void leftSemiBlock(BloqueDES* semiBlock, BloqueDES* fullBlock);
void rightSemiBlock(BloqueDES* semiBlock, BloqueDES* fullBlock);
void permChoice1(BloqueDES* new, BloqueDES* old);
void LCS(BloqueDES* new, BloqueDES* old, int nRound, int flag);
void permChoice2(BloqueDES* new, BloqueDES* old);
void singleRoundGeneral(BloqueDES* newLeft, BloqueDES* newRight, BloqueDES* oldLeft, BloqueDES* oldRight, BloqueDES* key, int nRound, short unsigned int*** Sbox);
void selectDES(BloqueDES* new, BloqueDES* old, const unsigned short* indices, int length);
void shiftLeftDES(BloqueDES* new, BloqueDES* old, int shift);
void shiftRightDES(BloqueDES* new, BloqueDES* old, int shift);
void expansion(BloqueDES* new, BloqueDES* old);
void xorDES(BloqueDES* new, BloqueDES* old1, BloqueDES* old2, int length);
void SBoxGeneral(BloqueDES* new, BloqueDES* old, short unsigned int*** Sbox);
void permutation(BloqueDES* new, BloqueDES* old);
int naleatorio(int a, int b);
unsigned short*** allocSboxes();
void freeSboxes(unsigned short*** Sboxes);
void printUsage(char* message);
void freeSboxes(unsigned short*** Sboxes);
void getSboxes(unsigned short*** Sboxes);








