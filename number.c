/*--------------------------------------------------------------------------
LIBRARIES
--------------------------------------------------------------------------*/
#include "number.h"
#include <stdlib.h>


/*--------------------------------------------------------------------------
FUNCTIONS
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
Translates a character into the corresponding array of eight binary digits
- Input:
	+ Array for the binary characters
	+ Character to translate
- Output: Void
--------------------------------------------------------------------------*/
void charToBin(uint8_t* bin, uint8_t c) {

	int i;

	for (i=7; i>=0; i--) {
		bin[i] = c%2;
		c = c/2;
	}
}


/*--------------------------------------------------------------------------
Translates a character into the corresponding array of hexadecimal digits
- Input:
	+ Array of two characters for the hexadecimal characters
	+ Character to translate
- Output: Void
--------------------------------------------------------------------------*/
void charToHex(char* hex, uint8_t c) {

	uint8_t bin[8];

	charToBin(bin,c);
	binToHex(hex,bin);
	binToHex(hex+1,bin+4);
}


/*--------------------------------------------------------------------------
Translates a long integer into an array of binary digits
- Input:
	+ Array of nDigits characters for the integer binary digits
	+ Integer to translate
	+ Number of binary digits
- Output: Void
--------------------------------------------------------------------------*/
void intToBin(uint8_t* bin, unsigned long int integer, int nDigits) {

	int i;

	for (i=nDigits-1; i>=0; i--) {
		bin[i] = integer%2;
		integer = integer/2;
	}
}


/*--------------------------------------------------------------------------
Translates an array of eight binary digits into the corresponding character
- Input:
	+ Pointer to the character
	+ Array with the binary characters
- Output: Void
--------------------------------------------------------------------------*/
void binToChar(uint8_t* c, uint8_t* bin) {

	int i;
	int pow = 1;

	(*c) = 0;

	for (i=7; i>=0; i--) {
		(*c) += pow*bin[i];
		pow *= 2;
	}
}


/*--------------------------------------------------------------------------
Translates an array of four binary digits into the corresponding hexadecimal
character
- Input:
	+ Pointer to the character
	+ Array with the binary characters
- Output: Void
--------------------------------------------------------------------------*/
void binToHex(char* c, uint8_t* bin) {

	int i;
	int value = 0;
	int pow = 1;

	for (i=3; i>=0; i--) {
		value += pow*bin[i];
		pow *= 2;
	}

	if (value < 10)
		(*c) = '0' + value;
	else
		(*c) = 'A' + value - 10;
}


/*--------------------------------------------------------------------------
Checks whether a character is a hexadecimal character ('0'-'9', 'A'-'F' or
'a'-'f')
- Input: Potential hexadecimal character
- Output:
	+ 1 if the character is hexadecimal
	+ 0 otherwise
--------------------------------------------------------------------------*/
int isHex(char hex) {

	if (((hex >= '0') && (hex <= '9')) || ((hex >= 'A') && (hex <= 'F')) || ((hex >= 'a') && (hex <= 'f')))
		return 1;

	return 0;
}


/*--------------------------------------------------------------------------
Translates an array of two hexadecimal characters into the corresponding
character
- Input:
	+ Pointer to the character
	+ Array with the hexadecimal characters
- Output: Void
--------------------------------------------------------------------------*/
void hexToChar(uint8_t* c, char* hex) {

	/* Least significant four bits */
	if ((hex[1] >= '0') && (hex[1] <= '9'))
		(*c) = hex[1] - '0';
	else if ((hex[1] >= 'A') && (hex[1] <= 'F'))
		(*c) = hex[1] + 10 - 'A';
	else if ((hex[1] >= 'a') && (hex[1] <= 'f'))
		(*c) = hex[1] + 10 - 'a';

	/* Most significant four bits */
	if ((hex[0] >= '0') && (hex[0] <= '9'))
		(*c) += 16*(hex[0] - '0');
	else if ((hex[0] >= 'A') && (hex[0] <= 'F'))
		(*c) += 16*(hex[0] + 10 - 'A');
	else if ((hex[0] >= 'a') && (hex[0] <= 'f'))
		(*c) += 16*(hex[0] + 10 - 'a');

}


/*--------------------------------------------------------------------------
Translates a hexadecimal character into the corresponding array of four
binary digits
- Input:
	+ Array for the binary characters
	+ Character to translate
- Output: Void
--------------------------------------------------------------------------*/
void hexToBin(uint8_t* bin, char hex) {

	int i, decValue = 0;

	if ((hex >= '0') && (hex <= '9'))
		decValue = hex - '0';
	else if ((hex >= 'A') && (hex <= 'Z'))
		decValue = hex + 10 - 'A';
	else if ((hex >= 'a') && (hex <= 'z'))
		decValue = hex + 10 - 'a';

	for (i=3; i>=0; i--) {
		bin[i] = decValue%2;
		decValue = decValue/2;
	}
}


/*--------------------------------------------------------------------------
Generates a pseudo-random integer in the range lowerLimit-upperLimit
- Input:
	+ Lower limit
	+ Upper limit
- Output: Random integer generated
--------------------------------------------------------------------------*/
int randomInt(int lowerLimit, int upperLimit) {

	if (lowerLimit >= upperLimit)
		return lowerLimit;

	return lowerLimit + (rand() % (upperLimit-lowerLimit+1));
}

