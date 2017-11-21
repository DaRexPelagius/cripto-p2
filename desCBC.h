/*--------------------------------------------------------------------------
LIBRARIES
--------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>


/*--------------------------------------------------------------------------
CONSTANTS
--------------------------------------------------------------------------*/
#define ERR_N_ARGS -1
#define ERR_FLAGS -2
#define BUFFER_SIZE 100
#define SIZE_OF_CHAR 8
#define MAX_TEXT_LENGTH 10000
#define MAX_NAME_LENGTH 256
#define PADDING_CHAR '.'
#define HEX_FORMAT 'h'
#define BIN_FORMAT 'b'
#define ASCII_FORMAT 'a'
#define SAC_FLAG 1
#define BIC_FLAG 2
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


int getArgs(int nArgs, char** args, int* flag, char* keyBuffer, int* blockSize, char* ivBuffer, int* ivLength, char* inputFormat, char* outputFormat, char* inputFileName, int* flagInput, char* outputFileName, int* flagOutput);
int getInteger(int nArgs, char** args, int* integer, char* flag, int flagLength);
int getString(int nArgs, char** args, char* string, char* flag, int flagLength);
int getFormat(int nArgs, char** args, char* format, char* flag, int flagLength);
void printOutput(FILE* outputFile, char format, char* text, int textLength);
int initCFB(char* keyBuffer, char* ivBuffer, int ivLength, int blockSize, CFBparameters* parameters);
void padding(char* text, int* textLength, int number, char padChar);
void printOutput(FILE* outputFile, char format, char* text, int textLength);
int readInput(FILE* inputFile, char format, char* text, int maxLength);
void printKey(FILE* outputFile, DESblock* key);
void CFBmode(int flag, char* plaintext, char* ciphertext, int textLength, CFBparameters* parameters)
int validKey(DESblock* key);
int hexaToBlock(DESblock* newBlock, char* string, int length);
void charToHex(char* hex, uint8_t c);
void newKey(DESblock* key);
void printHexaBlock(FILE* outputFile, DESblock* b, int blockSize, char* text);void hexToChar(uint8_t* c, char* hex);
void hexToChar(uint8_t* c, char* hex);
void textToBlock(DESblock* b, char* text, int blockSize);
void CFBstep(int flag, DESblock* plainBlock, DESblock* cipherBlock, CFBparameters* parameters);
void blockToText(char* text, DESblock* b, int blockSize);
void shiftRegister(DESblock* output, DESblock* input, int shift);
void charToBin(uint8_t* bin, uint8_t c);
void binToHex(char* c, uint8_t* bin);
void hexToBin(uint8_t* bin, char hex);
void DES(DESblock* output, DESblock* input, DESblock* key, int flag);
void xorDES(DESblock* new, DESblock* old1, DESblock* old2, int length);
void binToChar(uint8_t* c, uint8_t* bin);
void initialPerm(DESblock* new, DESblock* old);
void leftSemiBlock(DESblock* semiBlock, DESblock* fullBlock);
void rightSemiBlock(DESblock* semiBlock, DESblock* fullBlock;
void permChoice1(DESblock* new, DESblock* old);
void LCS(DESblock* new, DESblock* old, int nRound, int flag);
void permChoice2(DESblock* new, DESblock* old);
void singleRound(DESblock* newLeft, DESblock* newRight, DESblock* oldLeft, DESblock* oldRight, DESblock* key, int nRound);
void copyBlock(DESblock* new, DESblock* old, int length);
void invInitialPerm(DESblock* new, DESblock* old);
void selectDES(DESblock* new, DESblock* old, const unsigned short* indices, int length);
void expansion(DESblock* new, DESblock* old);
void SBox(DESblock* new, DESblock* old);
