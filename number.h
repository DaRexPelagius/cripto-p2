#ifndef __NUMBER_H
#define __NUMBER_H


/*--------------------------------------------------------------------------
LIBRARIES
--------------------------------------------------------------------------*/
#include <stdint.h>


/*--------------------------------------------------------------------------
PUBLIC FUNCTION DEFINITIONS
--------------------------------------------------------------------------*/
void charToBin(uint8_t* bin, uint8_t c);
void charToHex(char* hex, uint8_t c);
void intToBin(uint8_t* bin, unsigned long int integer, int nDigits);
void binToChar(uint8_t* c, uint8_t* bin);
void binToHex(char* c, uint8_t* bin);
int isHex(char hex);
void hexToChar(uint8_t* c, char* hex);
void hexToBin(uint8_t* bin, char hex);
int randomInt(int lowerLimit, int upperLimit);

#endif

