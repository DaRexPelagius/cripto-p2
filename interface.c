/*--------------------------------------------------------------------------
LIBRARIES
--------------------------------------------------------------------------*/
#include "interface.h"
#include "number.h"
#include <ctype.h>


/*--------------------------------------------------------------------------
FUNCTIONS
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
Gets the encryption/decryption flag from the command-line arguments
- Input:
	+ Number of arguments
	+ Array of command-line arguments
	+ Pointer to the flag for encryption/decryption
- Output:
	+ 1 if the flag appears once
	+ 0 if the flag does not appear
	+ -1 if the flag is repeated
--------------------------------------------------------------------------*/
int getFlag(int nArgs, char** args, int* flag) {

	int i;
	int found = 0;

	for (i=1; i<nArgs; i++)
		if ((strncmp(args[i],"-C",2) == 0) && (strlen(args[i]) == 2)) {
			if (found)
				return -1;
			else {
				*flag = ENCRYPT_FLAG;
				found = 1;
			}
		}
		else if (strncmp(args[i],"-D",2) == 0) {
			if (found)
				return -1;
			else {
				*flag = DECRYPT_FLAG;
				found = 1;
			}
		}

	return found;
}


/*--------------------------------------------------------------------------
Gets the criterion (SAC or BIC) flag from the command-line arguments
- Input:
	+ Number of arguments
	+ Array of command-line arguments
	+ Pointer to the criterion flag
- Output:
	+ 1 if the flag appears once
	+ 0 if the flag does not appear
	+ -1 if the flag is repeated
--------------------------------------------------------------------------*/
int getCriterionFlag(int nArgs, char** args, int* criterionFlag) {

	int i;
	int found = 0;

	for (i=1; i<nArgs; i++)
		if ((strncmp(args[i],"-S",2) == 0) && (strlen(args[i]) == 2)) {
			if (found)
				return -1;
			else {
				*criterionFlag = SAC_FLAG;
				found = 1;
			}
		}
		else if (strncmp(args[i],"-B",2) == 0) {
			if (found)
				return -1;
			else {
				*criterionFlag = BIC_FLAG;
				found = 1;
			}
		}

	return found;
}


/*--------------------------------------------------------------------------
Gets an integer from the command-line arguments
- Input:
	+ Number of arguments
	+ Array of command-line arguments
	+ Pointer to the integer variable
	+ String with the flag preceding the integer value
	+ Lenght of the string with such flag
- Output:
	+ 1 if the integer flag appears once and is followed by an integer
	+ 0 if the integer flag does not appear
	+ -1 if the integer flag is repeated or is not followed by an integer
--------------------------------------------------------------------------*/
int getInteger(int nArgs, char** args, int* integer, char* flag, int flagLength) {

	int i;
	int found = 0;

	for (i=1; i<=nArgs-2; i++)
		if ((strncmp(args[i],flag,flagLength) == 0) && (strlen(args[i]) == flagLength)) {
			if (found)
				return -1;
			else {
				*integer = atoi(args[i+1]);
				if ((*integer == 0) && !isdigit(args[i+1][0]))
					return -1;
				found = 1;
			}
		}

	return found;
}


/*--------------------------------------------------------------------------
Gets a string from the command-line arguments
- Input:
	+ Number of arguments
	+ Array of command-line arguments
	+ String variable
	+ String with the flag preceding the string
	+ Lenght of the string with such flag
- Output:
	+ 1 if the string flag appears once
	+ 0 if the string flag does not appear
	+ -1 if the string flag is repeated
--------------------------------------------------------------------------*/
int getString(int nArgs, char** args, char* string, char* flag, int flagLength) {

	int i;
	int found = 0;

	for (i=1; i<=nArgs-2; i++)
		if ((strncmp(args[i],flag,flagLength) == 0) && (strlen(args[i]) == flagLength)) {
			if (found)
				return -1;
			else {
				strcpy(string,args[i+1]);
				found = 1;
			}
		}

	return found;
}


/*--------------------------------------------------------------------------
Gets the encryption/decryption flag from the command-line arguments
- Input:
	+ Number of arguments
	+ Array of command-line arguments
	+ String variable
	+ String with the flag preceding the string
	+ Lenght of the string with such flag
- Output:
	+ 1 if the format flag appears once
	+ 0 if the format flag does not appear
	+ -1 if the format flag is repeated or does not match a correct format
--------------------------------------------------------------------------*/
int getFormat(int nArgs, char** args, char* format, char* flag, int flagLength) {

	int i;
	int found = 0;

	for (i=1; i<=nArgs-2; i++)
		if ((strncmp(args[i],flag,flagLength) == 0) && (strlen(args[i]) == flagLength)) {
			if (found)
				return -1;
			else {
				if ((strlen(args[i+1]) == 1) && (args[i+1][0] == HEX_FORMAT)) {
					*format = HEX_FORMAT;
					found = 1;
				}
				else if ((strlen(args[i+1]) == 1) && (args[i+1][0] == BIN_FORMAT)) {
					*format = BIN_FORMAT;
					found = 1;
				}
				else if ((strlen(args[i+1]) == 1) && (args[i+1][0] == ASCII_FORMAT)) {
					*format = ASCII_FORMAT;
					found = 1;
				}
				else
					return -1;
			}
		}

	return found;
}


/*--------------------------------------------------------------------------
Closes the files used by the program
- Input:
	+ Pointer to the input file to close
	+ Pointer to the output file to close
- Output: Void
--------------------------------------------------------------------------*/
void closeFiles(FILE* inputFile, FILE* outputFile) {

	if (inputFile && (inputFile != stdin))
		fclose(inputFile);
	if (outputFile && (outputFile != stdout))
		fclose(outputFile);
}


/*--------------------------------------------------------------------------
Reads the input text, which may be in hexadecimal or ASCII format, and
translates it into an array of characters
- Input:
	+ Pointer to the file stream with the input text
	+ Input format (hexadecimal or ASCII)
	+ Array for characters that will be read
	+ Maximum length
- Output: Number of valid characters read
--------------------------------------------------------------------------*/
int readInput(FILE* inputFile, char format, char* text, int maxLength) {

	char c;
	uint8_t auxC;
	char hex[2];
	int n = 0;

	if (format == ASCII_FORMAT)
		while (((c = fgetc(inputFile)) != EOF) && (inputFile != stdin || c != '\n') && (n < maxLength)) {
			text[n] = c;
			n++;
		}

	else if (format == HEX_FORMAT)

		while (1) {

			/* Reads first half of the byte */
			hex[0] = fgetc(inputFile);
			while (!isHex(hex[0])) {
				/* End of input */
				if ((hex[0] == EOF) || ((inputFile == stdin) && (hex[0] == '\n')))
					return n;
				hex[0] = fgetc(inputFile);
			}

			/* Reads second half of the byte */
			hex[1] = fgetc(inputFile);
			while (!isHex(hex[1])) {
				/* End of input with an incomplete byte => Padding */
				if ((hex[1] == EOF) || ((inputFile == stdin) && (hex[1] == '\n'))) {
					hex[1] = '0';
					hexToChar(&auxC,hex);
					text[n] = auxC;
					n++;
					return n;
				}
				hex[1] = fgetc(inputFile);
			}

			/* Translates the complete byte */
			hexToChar(&auxC,hex);
			text[n] = auxC;
			n++;
		}

	return n;
}


/*--------------------------------------------------------------------------
Pads the input text with the padding character until its length (in bits)
reaches a multiple of the given number
- Input:
	+ Text
	+ Text length, which is updated
	+ Number the desired length is a multiple of
	+ Padding character
- Output: Void
--------------------------------------------------------------------------*/
void padding(char* text, int* textLength, int number, char padChar) {

	while ((*textLength)*8 % number != 0)
		text[(*textLength)++] = padChar;
}


/*--------------------------------------------------------------------------
Writes the output text from a string of characters
- Input:
	+ Pointer to the file stream in which to write the text
	+ Output format (hexadecimal or ASCII)
	+ String to be written
	+ Text length
- Output: Void
--------------------------------------------------------------------------*/
void printOutput(FILE* outputFile, char format, char* text, int textLength) {

	int i;
	char hex[2];

	if (format == ASCII_FORMAT)
		for (i=0; i<textLength; i++)
			fputc(text[i],outputFile);
	else if (format == HEX_FORMAT)
		for (i=0; i<textLength; i++) {
			charToHex(hex,text[i]);
			fputc(hex[0],outputFile);
			fputc(hex[1],outputFile);
		}

	fputc('\n',outputFile);
}

