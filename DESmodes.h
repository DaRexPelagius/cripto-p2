#ifndef __DESMODES_H
#define __DESMODES_H


/*--------------------------------------------------------------------------
LIBRARIES
--------------------------------------------------------------------------*/
#include "DES.h"
#include <stdio.h>


/*--------------------------------------------------------------------------
CONSTANTS
--------------------------------------------------------------------------*/
#define ERR_KEY_FORMAT -1
#define ERR_KEY -2
#define ERR_IV_FORMAT -3


/*--------------------------------------------------------------------------
DATA STRUCTURES
--------------------------------------------------------------------------*/

/* Parameters for DES algorithm in CFB mode */
typedef struct {
	DESblock key;
	DESblock SR;
	DESblock IV;
	int ivLength;
	int blockSize;
} CFBparameters;

/* Parameters for DES algorithm in OFB mode */
typedef struct {
	DESblock key;
	DESblock SR;
	DESblock IV;
	int ivLength;
	int blockSize;
} OFBparameters;

/* Parameters for triple key, triple DES algorithm in CFB mode */
typedef struct {
	DESblock key1;
	DESblock key2;
	DESblock key3;
	DESblock SR;
	DESblock IV;
	int ivLength;
	int blockSize;
} tripleCFBparameters;


/*--------------------------------------------------------------------------
PUBLIC FUNCTION DEFINITIONS
--------------------------------------------------------------------------*/
int initCFB(char* keyBuffer, char* ivBuffer, int ivLength, int blockSize, CFBparameters* parameters);
void CFBmode(int flag, char* plaintext, char* ciphertext, int textLength, CFBparameters* parameters);
int initOFB(char* keyBuffer, char* ivBuffer, int ivLength, int blockSize, OFBparameters* parameters);
void OFBmode(int flag, char* plaintext, char* ciphertext, int textLength, OFBparameters* parameters);
int initTripleCFB(char* keyBuffer1, char* keyBuffer2, char* keyBuffer3, char* ivBuffer, int ivLength, int blockSize, tripleCFBparameters* parameters);
void tripleCFBmode(int flag, char* plaintext, char* ciphertext, int textLength, tripleCFBparameters* parameter);

#endif

