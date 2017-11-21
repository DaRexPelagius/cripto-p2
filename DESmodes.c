/*--------------------------------------------------------------------------
LIBRARIES
--------------------------------------------------------------------------*/
#include "DESmodes.h"
#include "number.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

/*--------------------------------------------------------------------------
FUNCTION DEFINITIONS
--------------------------------------------------------------------------*/
void CFBstep(int flag, DESblock* plainBlock, DESblock* cipherBlock, CFBparameters* parameters);
void OFBstep(int flag, DESblock* plainBlock, DESblock* cipherBlock, OFBparameters* parameters);
void tripleCFBstep(int flag, DESblock* plainBlock, DESblock* cipherBlock, tripleCFBparameters* parameters);
void shiftRegister(DESblock* output, DESblock* input, int shift);


/*--------------------------------------------------------------------------
FUNCTIONS
--------------------------------------------------------------------------*/

/*··········································································
CIPHER FEEDBACK MODE (CFB)
··········································································*/

/*--------------------------------------------------------------------------
Initialises DES parameters for CFB mode, validating the key and the
initialisation vector. If the key buffer is NULL, a random key is generated
- Input:
	+ Key buffer
	+ Initialisation vector buffer
	+ Initialisation vector length
	+ Pointer to the CFB parameters
- Output:
	+ 0 if the parameters are valid
	+ -1 otherwise
--------------------------------------------------------------------------*/
int initCFB(char* keyBuffer, char* ivBuffer, int ivLength, int blockSize, CFBparameters* parameters) {

	/* Key initialisation and validation */
	if (keyBuffer) {
		if (hexaToBlock(&(parameters->key),keyBuffer,DES_KEY_SIZE) == -1)
			return ERR_KEY_FORMAT;
		else if (!validKey(&(parameters->key)))
			return ERR_KEY;
	}
	else {
		srand(time(NULL));
		newKey(&(parameters->key));
	}

	/* IV initialisation and validation */
	if (hexaToBlock(&(parameters->IV),ivBuffer,ivLength) == -1)
		return ERR_IV_FORMAT;
	parameters->ivLength = ivLength;

	/* Block size */
	parameters->blockSize = blockSize;

	return 0;
}


/*--------------------------------------------------------------------------
Cipher Feedback Mode for DES algorithm
- Input:
	+ Flag for encryption (ENCRYPT_FLAG) / decryption (DECRYPT_FLAG)
	+ Text array of plaintext
	+ Text array of ciphertext
	+ Text length (multiple of the block size)
	+ CFB parameters
- Output: Void
--------------------------------------------------------------------------*/
void CFBmode(int flag, char* plaintext, char* ciphertext, int textLength, CFBparameters* parameters) {
	
	DESblock plainBlock, cipherBlock;
	int i;
	int l = parameters->ivLength;
	int s = parameters->blockSize;
	int nBlocks = textLength*8/s;

	/* Initial shift register content */
	for (i=1; i<=DES_BLOCK_SIZE-l; i++)
		parameters->SR.block[i] = 0;
	for (i=1; i<=l; i++)
		parameters->SR.block[DES_BLOCK_SIZE-l+i] = parameters->IV.block[i];

	/* Encryption/decryption by blocks */
	if (flag == ENCRYPT_FLAG) {
		for (i=0; i<nBlocks; i++) {
			textToBlock(&plainBlock, plaintext + i*s/8, s);
			CFBstep(flag,&plainBlock,&cipherBlock,parameters);
			blockToText(ciphertext + i*s/8, &cipherBlock, s);
		}
	}
	else {
		for (i=0; i < nBlocks; i++) {
			textToBlock(&cipherBlock, ciphertext + i*s/8, s);
			CFBstep(flag,&plainBlock,&cipherBlock,parameters);
			blockToText(plaintext + i*s/8, &plainBlock, s);
		}
	}
}


/*--------------------------------------------------------------------------
Cipher Feedback Mode step for DES algorithm
- Input:
	+ Flag for encryption (ENCRYPT_FLAG) / decryption (DECRYPT_FLAG)
	+ Block of plaintext
	+ Block of ciphertext
	+ CFB parameters
- Output: Void
--------------------------------------------------------------------------*/
void CFBstep(int flag, DESblock* plainBlock, DESblock* cipherBlock, CFBparameters* parameters) {

	int s = parameters->blockSize;
	DESblock outputDES;

	/* DES algorithm (input from shift register) */
	DES(&outputDES,&(parameters->SR),&(parameters->key),ENCRYPT_FLAG);

	/* XOR with the s most significant bits */
	if (flag == ENCRYPT_FLAG)
		xorDES(cipherBlock,plainBlock,&outputDES,s);
	else
		xorDES(plainBlock,cipherBlock,&outputDES,s);

	/* Shift register ready for the next block */
	shiftRegister(&(parameters->SR),cipherBlock,s);
}


/*··········································································
OUTPUT FEEDBACK MODE (OFB)
··········································································*/

/*--------------------------------------------------------------------------
Initialises DES parameters for OFB mode, validating the key and the
initialisation vector. If the key buffer is NULL, a random key is generated
- Input:
	+ Key buffer
	+ Initialisation vector buffer
	+ Initialisation vector length
	+ Pointer to the OFB parameters
- Output:
	+ 0 if the parameters are valid
	+ -1 otherwise
--------------------------------------------------------------------------*/
int initOFB(char* keyBuffer, char* ivBuffer, int ivLength, int blockSize, OFBparameters* parameters) {

	/* Key initialisation and validation */
	if (keyBuffer) {
		if (hexaToBlock(&(parameters->key),keyBuffer,DES_KEY_SIZE) == -1)
			return ERR_KEY_FORMAT;
		else if (!validKey(&(parameters->key)))
			return ERR_KEY;
	}
	else {
		srand(time(NULL));
		newKey(&(parameters->key));
	}

	/* IV initialisation and validation */
	if (hexaToBlock(&(parameters->IV),ivBuffer,ivLength) == -1)
		return ERR_IV_FORMAT;
	parameters->ivLength = ivLength;

	/* Block size */
	parameters->blockSize = blockSize;

	return 0;
}


/*--------------------------------------------------------------------------
Output Feedback Mode for DES algorithm
- Input:
	+ Flag for encryption (ENCRYPT_FLAG) / decryption (DECRYPT_FLAG)
	+ Text array of plaintext
	+ Text array of ciphertext
	+ Text length (multiple of the block size)
	+ CFB parameters
- Output: Void
--------------------------------------------------------------------------*/
void OFBmode(int flag, char* plaintext, char* ciphertext, int textLength, OFBparameters* parameters) {
	
	DESblock plainBlock, cipherBlock;
	int i;
	int l = parameters->ivLength;
	int s = parameters->blockSize;
	int nBlocks = textLength*8/s;

	/* Initial shift register content */
	for (i=1; i<=DES_BLOCK_SIZE-l; i++)
		parameters->SR.block[i] = 0;
	for (i=1; i<=l; i++)
		parameters->SR.block[DES_BLOCK_SIZE-l+i] = parameters->IV.block[i];

	/* Encryption/decryption by blocks */
	if (flag == ENCRYPT_FLAG) {
		for (i=0; i<nBlocks; i++) {
			textToBlock(&plainBlock, plaintext + i*s/8, s);
			OFBstep(flag,&plainBlock,&cipherBlock,parameters);
			blockToText(ciphertext + i*s/8, &cipherBlock, s);
		}
	}
	else {
		for (i=0; i < nBlocks; i++) {
			textToBlock(&cipherBlock, ciphertext + i*s/8, s);
			OFBstep(flag,&plainBlock,&cipherBlock,parameters);
			blockToText(plaintext + i*s/8, &plainBlock, s);
		}
	}
}


/*--------------------------------------------------------------------------
Output Feedback Mode step for DES algorithm
- Input:
	+ Flag for encryption (ENCRYPT_FLAG) / decryption (DECRYPT_FLAG)
	+ Block of plaintext
	+ Block of ciphertext
	+ CFB parameters
- Output: Void
--------------------------------------------------------------------------*/
void OFBstep(int flag, DESblock* plainBlock, DESblock* cipherBlock, OFBparameters* parameters) {

	int s = parameters->blockSize;
	DESblock outputDES;

	/* DES algorithm (input from shift register) */
	DES(&outputDES,&(parameters->SR),&(parameters->key),ENCRYPT_FLAG);

	/* XOR with the s most significant bits */
	if (flag == ENCRYPT_FLAG)
		xorDES(cipherBlock,plainBlock,&outputDES,s);
	else
		xorDES(plainBlock,cipherBlock,&outputDES,s);

	/* Shift register ready for the next block */
	shiftRegister(&(parameters->SR),&outputDES,s);
}


/*··········································································
TRIPLE DES WITH CFB MODE
··········································································*/

/*--------------------------------------------------------------------------
Initialises triple DES parameters for OFB mode, validating the keys and the
initialisation vector. If any key buffer is NULL, a corresponding random key
is generated
- Input:
	+ Key buffers
	+ Initialisation vector buffer
	+ Initialisation vector length
	+ Pointer to the OFB parameters
- Output:
	+ 0 if the parameters are valid
	+ -1 otherwise
--------------------------------------------------------------------------*/
int initTripleCFB(char* keyBuffer1, char* keyBuffer2, char* keyBuffer3, char* ivBuffer, int ivLength, int blockSize, tripleCFBparameters* parameters) {

	int initSeed = 0;

	/* Key initialisations and validation */
	if (keyBuffer1) {
		if (hexaToBlock(&(parameters->key1),keyBuffer1,DES_KEY_SIZE) == -1)
			return ERR_KEY_FORMAT;
		else if (!validKey(&(parameters->key1)))
			return ERR_KEY;
	}
	else {
		srand(time(NULL));
		initSeed = 1;
		newKey(&(parameters->key1));
	}

	if (keyBuffer2) {
		if (hexaToBlock(&(parameters->key2),keyBuffer2,DES_KEY_SIZE) == -1)
			return ERR_KEY_FORMAT;
		else if (!validKey(&(parameters->key2)))
			return ERR_KEY;
	}
	else {
		if (!initSeed)
			srand(time(NULL));
		newKey(&(parameters->key2));
	}

	if (keyBuffer3) {
		if (hexaToBlock(&(parameters->key3),keyBuffer3,DES_KEY_SIZE) == -1)
			return ERR_KEY_FORMAT;
		else if (!validKey(&(parameters->key3)))
			return ERR_KEY;
	}
	else {
		if (!initSeed)
			srand(time(NULL));
		newKey(&(parameters->key3));
	}

	/* IV initialisation and validation */
	if (hexaToBlock(&(parameters->IV),ivBuffer,ivLength) == -1)
		return ERR_IV_FORMAT;
	parameters->ivLength = ivLength;

	/* Block size */
	parameters->blockSize = blockSize;

	return 0;
}


/*--------------------------------------------------------------------------
Triple Cipher Feedback Mode for DES algorithm
- Input:
	+ Flag for encryption (ENCRYPT_FLAG) / decryption (DECRYPT_FLAG)
	+ Text array of plaintext
	+ Text array of ciphertext
	+ Text length (multiple of the block size)
	+ CFB parameters
- Output: Void
--------------------------------------------------------------------------*/
void tripleCFBmode(int flag, char* plaintext, char* ciphertext, int textLength, tripleCFBparameters* parameters) {

	DESblock plainBlock, cipherBlock;
	int i;
	int l = parameters->ivLength;
	int s = parameters->blockSize;
	int nBlocks = textLength*8/s;

	/* Initial shift register content */
	for (i=1; i<=DES_BLOCK_SIZE-l; i++)
		parameters->SR.block[i] = 0;
	for (i=1; i<=l; i++)
		parameters->SR.block[DES_BLOCK_SIZE-l+i] = parameters->IV.block[i];

	/* Encryption/decryption by blocks */
	if (flag == ENCRYPT_FLAG) {
		for (i=0; i<nBlocks; i++) {
			textToBlock(&plainBlock, plaintext + i*s/8, s);
			tripleCFBstep(flag,&plainBlock,&cipherBlock,parameters);
			blockToText(ciphertext + i*s/8, &cipherBlock, s);
		}
	}
	else {
		for (i=0; i < nBlocks; i++) {
			textToBlock(&cipherBlock, ciphertext + i*s/8, s);
			tripleCFBstep(flag,&plainBlock,&cipherBlock,parameters);
			blockToText(plaintext + i*s/8, &plainBlock, s);
		}
	}
}


/*--------------------------------------------------------------------------
Triple Cipher Feedback Mode step for DES algorithm
- Input:
	+ Flag for encryption (ENCRYPT_FLAG) / decryption (DECRYPT_FLAG)
	+ Block of plaintext
	+ Block of ciphertext
	+ Triple DES-CFB parameters
- Output: Void
--------------------------------------------------------------------------*/
void tripleCFBstep(int flag, DESblock* plainBlock, DESblock* cipherBlock, tripleCFBparameters* parameters) {

	int s = parameters->blockSize;
	DESblock outputDES1, outputDES2;

	/* DES algorithm (first input from shift register) */
	DES(&outputDES1,&(parameters->SR),&(parameters->key1),ENCRYPT_FLAG);
	DES(&outputDES2,&outputDES1,&(parameters->key2),DECRYPT_FLAG);
	DES(&outputDES1,&outputDES2,&(parameters->key3),ENCRYPT_FLAG);

	/* XOR with the s most significant bits; shift register ready for the next block */
	if (flag == ENCRYPT_FLAG)
		xorDES(cipherBlock,plainBlock,&outputDES1,s);
	else
		xorDES(plainBlock,cipherBlock,&outputDES1,s);

	/* Shift register ready for the next block */
	shiftRegister(&(parameters->SR),cipherBlock,s);
}


/*··········································································
SHIFT REGISTER
··········································································*/

/*--------------------------------------------------------------------------
Shifts the (DES_BLOCK_SIZE-shift) least significant bits of a block
according to the given shift, introducing in the remaining 'shift' bits
those from the input block
- Input:
	+ New block
	+ Input block
	+ Length
	+ Shift
- Output: Void
--------------------------------------------------------------------------*/
void shiftRegister(DESblock* output, DESblock* input, int shift) {

	int i;

	for (i=1; i <= DES_BLOCK_SIZE-shift; i++)
		output->block[i] = output->block[i+shift];
	for (i=1; i <= shift; i++)
		output->block[i+DES_BLOCK_SIZE-shift] = input->block[i];
}

