#ifndef __INTERFACE_H
#define __INTERFACE_H


/*--------------------------------------------------------------------------
LIBRARIES
--------------------------------------------------------------------------*/
#include "DES.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


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


/*--------------------------------------------------------------------------
PUBLIC FUNCTION DEFINITIONS
--------------------------------------------------------------------------*/
int getFlag(int nArgs, char** args, int* flag);
int getCriterionFlag(int nArgs, char** args, int* criterionFlag);
int getInteger(int nArgs, char** args, int* integer, char* flag, int flagLength);
int getString(int nArgs, char** args, char* string, char* flag, int flagLength);
int getFormat(int nArgs, char** args, char* format, char* flag, int flagLength);
void closeFiles(FILE* inputFile, FILE* outputFile);
int readInput(FILE* inputFile, char format, char* text, int maxLength);
void padding(char* text, int* textLength, int number, char padChar);
void printOutput(FILE* outputFile, char format, char* text, int textLength);

#endif

