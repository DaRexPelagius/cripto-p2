#include "desCBC.h"


/*--------------------------------------------------------------------------
PARAMETERS
--------------------------------------------------------------------------*/

/* PC1 "permutation" */
static const unsigned short PC1[BITS_IN_PC1] = { 
	57, 49, 41, 33, 25, 17, 9,
	1, 58, 50, 42, 34, 26, 18,
	10, 2, 59, 51, 43, 35, 27,
	19, 11, 3, 60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15,
	7, 62, 54, 46, 38, 30, 22,
	14, 6, 61, 53, 45, 37, 29,
	21, 13, 5, 28, 20, 12, 4
};

/* PC2 "permutation" */
static const unsigned short PC2[BITS_IN_PC2] = {
	14, 17, 11, 24, 1, 5,
	3, 28, 15, 6, 21, 10,
	23, 19, 12, 4, 26, 8,
	16, 7, 27, 20, 13, 2,
	41, 52, 31, 37, 47, 55,
	30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53,
	46, 42, 50, 36, 29, 32
};

/* Shifts in each of the key halves in each round (for encryption) */
static const unsigned short ROUND_SHIFTS[NUM_ROUNDS] = {
	1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

/* Shifts in each of the key halves in each round (for decryption) */
static const unsigned short ROUND_SHIFTS_DEC[NUM_ROUNDS] = {
	0, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

/* IP permutation */
static const unsigned short IP[BITS_IN_IP] = {
	58, 50, 42, 34, 26, 18, 10, 2,
	60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14, 6,
	64, 56, 48, 40, 32, 24, 16, 8,
	57, 49, 41, 33, 25, 17, 9, 1,
	59, 51, 43, 35, 27, 19, 11, 3,
	61, 53, 45, 37, 29, 21, 13, 5,
	63, 55, 47, 39, 31, 23, 15, 7
};

/* IP inverse */
static const unsigned short IP_INV[BITS_IN_IP] = {
	40, 8, 48, 16, 56, 24, 64, 32,
	39, 7, 47, 15, 55, 23, 63, 31,
	38, 6, 46, 14, 54, 22, 62, 30,
	37, 5, 45, 13, 53, 21, 61, 29,
	36, 4, 44, 12, 52, 20, 60, 28,
	35, 3, 43, 11, 51, 19, 59, 27,
	34, 2, 42, 10, 50, 18, 58, 26,
	33, 1, 41, 9, 49, 17, 57, 25
};

/* E expansion */
static const unsigned short E[BITS_IN_E] = {
	32, 1, 2, 3, 4, 5,
	4, 5, 6, 7, 8, 9,
	8, 9, 10, 11, 12, 13,
	12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21,
	20, 21, 22, 23, 24, 25,
	24, 25, 26, 27, 28, 29,
	28, 29, 30, 31, 32, 1
};

/* P permutation */
static const unsigned short P[BITS_IN_P] = {
	16, 7, 20, 21,
	29, 12, 28, 17,
	1, 15, 23, 26,
	5, 18, 31, 10,
	2, 8, 24, 14,
	32, 27, 3, 9,
	19, 13, 30, 6,
	22, 11, 4, 25
};

/* 32-bit swap */
static const unsigned short SWAP[BITS_IN_SWAP] = {
	33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48,
	49, 50, 51, 52, 53, 54, 55, 56,
	57, 58, 59, 60, 61, 62, 63, 64,
	1, 2, 3, 4, 5, 6, 7, 8,
	9, 10, 11, 12, 13, 14, 15, 16,
	17, 18, 19, 20, 21, 22, 23, 24,
	25, 26, 27, 28, 29, 30, 31, 32
};

/* S-boxes */
static const unsigned short S_BOXES[NUM_S_BOXES][ROWS_PER_SBOX][COLUMNS_PER_SBOX] = {
	{	{ 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7 },
		{ 0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8 },
		{ 4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0 },
		{ 15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13 } },
	{
		{ 15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10 },
		{ 3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5 },
		{ 0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15 },
		{ 13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9 }	},

	{	{ 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8 },
		{ 13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1 },
		{ 13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7 },
		{ 1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12 }	},

	{	{ 7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15 },
		{ 13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9 },
		{ 10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4 },
		{ 3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14 }	},
	{
		{ 2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9 },
		{ 14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6 },
		{ 4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14 },
		{ 11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3 }	},
	{	
		{ 12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11 },
		{ 10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8 },
		{ 9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6 },
		{ 4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13 }	},
	{
 		{ 4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1 },
		{ 13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6 },
		{ 1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2 },
		{ 6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12 }	},
	{
		{ 13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7 },
		{ 1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2 },
		{ 7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8 },
		{ 2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11 }	}
};

int main(int argc, char** argv) {

	int flag,blockSize,ivLength;
	char inputFormat,outputFormat;
	int flagInput,flagOutput;
	char keyBuffer[BUFFER_SIZE];
	char ivBuffer[BUFFER_SIZE];
	char inputFileName[MAX_NAME_LENGTH];
	char outputFileName[MAX_NAME_LENGTH];
	FILE* inputFile = NULL;
	FILE* outputFile = NULL;
	CFBparameters parameters;
	char plaintext[MAX_TEXT_LENGTH];
	char ciphertext[MAX_TEXT_LENGTH];
	int textLength;
	int initRet;

	//Comprobamos que los argumentos son los correctos
	if ((argc != 12) && (argc != 14) && (argc != 16) && (argc != 18)){
		printf("Número de argumentos incorrecto.\n");
		printf("Uso del programa: ./1 {-P | -I} [-i filein] [-o fileout].\n");	
		return -1;
	}
	//Como en la practica anterior usamos una funcion auxiliar para conseguir los argumentos
	if (getArgs(argc, argv, &flag, keyBuffer, &blockSize, ivBuffer, &ivLength, &inputFormat, &outputFormat, inputFileName, &flagInput, outputFileName, &flagOutput) == -1) {
		printf("Argumentos incorrectos.\n");
		printf("Uso del programa: ./1 {-P | -I} [-i filein] [-o fileout].\n");
		return -1;
	}
	
	//Ahora vamos a comprar que los argumentos del DES sean correctos..

	//Tamanyo de bloque entre los limites
	if ((blockSize < 1) || (blockSize > DES_BLOCK_SIZE)) {
		fprintf(stderr,"ERROR: Block size must be between 1 and %d\n",DES_BLOCK_SIZE);
		return -1;
	}
	//Tamanyo de bloque multiplo de 8
	if ((blockSize%8) != 0) {
		fprintf(stderr,"ERROR: Block size must be a multiple of 8\n");
		return -1;
	}
	//Vector de inicializacion correcto
	if ((ivLength < 1) || (ivLength > blockSize)) {
		fprintf(stderr,"ERROR: IV length must be between 1 and %d\n",blockSize);
		return -1;
	}
	//Vector de inicializacion multiplo de 8
	if ((ivLength%4) != 0) {
		fprintf(stderr,"ERROR: IV length must be a multiple of 4\n");
		return -1;
	}
	//Comprobamos que el archivo de entrada esta en un lenguaje que entendamos
	if ((inputFormat != HEX_FORMAT) && (inputFormat != ASCII_FORMAT)) {
		fprintf(stderr,"ERROR: Wrong input format\n");
		return -1;
	}
	//Comprobamos que el lenguaje de salida lo conocemos
	if ((outputFormat != HEX_FORMAT) && (outputFormat != ASCII_FORMAT)) {
		fprintf(stderr,"ERROR: Wrong input format\n");
		return -1;
	}
	
	//Si se indico un fichero de entrada lo abrimos
	if (entrada) {
		if ((fentrada = fopen(ficheroentrada, "r")) == NULL) {
			fprintf(stderr, "Error al abrir el fichero de entrada %s\n", ficheroentrada);
			return -1;
		}
	} else fentrada = stdin;
	//Si se indico un fichero de salida lo abrimos
	if (salida) {
		if ((fsalida = fopen(ficherosalida, "w")) == NULL) {
			fprintf(stderr, "Error al abrir el fichero de salida %s\n", ficherosalida);
			if (fentrada) fclose(fentrada);
			return -1;
		}
	} else fsalida = stdout;

	//Preparamos la estructura DES
	if (flag == ENCRYPT_FLAG) initRet = initCFB(NULL, ivBuffer, ivLength, blockSize, &parameters);
	else initRet = initCFB(keyBuffer, ivBuffer, ivLength, blockSize, &parameters);
	
	if (initRet != 0) {
		switch (initRet) {
			case ERR_KEY_FORMAT:
				fprintf(stderr,"ERROR: Wrong DES key format\n");
				break;
			case ERR_KEY:
				fprintf(stderr,"ERROR: Wrong DES key\n");
				break;
			case ERR_IV_FORMAT:
				fprintf(stderr,"ERROR: Wrong DES initialisation vector for CFB mode\n");
				break;
		}
		if (fentrada) fclose(fentrada);
		if (fsalida) fclose(fsalida);
		return -1;
	}

	//Imprimimos la clave de Encriptacion 
	if (flag == ENCRYPT_FLAG) printKey(stdout,&(parameters.key));

	//Si el programa se ejecuto como encriptacion, encriptamos
	if (flag == ENCRYPT_FLAG) {
		if (!flagInput) fprintf(stdout,"Plaintext:\n");
		textLength = readInput(inputFile,inputFormat,plaintext,MAX_TEXT_LENGTH);
		padding(plaintext,&textLength,blockSize,PADDING_CHAR);
		CFBmode(flag,plaintext,ciphertext,textLength,&parameters);
		if (!flagOutput) fprintf(stdout,"Ciphertext:\n");
		printOutput(outputFile,outputFormat,ciphertext,textLength);
	}

	//Si el programa se ejecuto como desencriptacion, desencriptamos
	else if (flag == DECRYPT_FLAG) {
		if (!flagInput) fprintf(stdout,"Ciphertext:\n");
		textLength = readInput(inputFile,inputFormat,ciphertext,MAX_TEXT_LENGTH);
		CFBmode(flag,plaintext,ciphertext,textLength,&parameters);
		if (!flagOutput) fprintf(stdout,"Plaintext:\n");
		printOutput(outputFile,outputFormat,plaintext,textLength);
	}

	if (fentrada) fclose(fentrada);
	if (fsalida) fclose(fsalida);

	return 0;
}

int getArgs(int nArgs, char** args, int* flag, char* keyBuffer, int* blockSize, char* ivBuffer, int* ivLength, char* inputFormat, char* outputFormat, char* inputFileName, int* flagInput, char* outputFileName, int* flagOutput) {

	if (getFlag(nArgs,args,flag) != 1)
		return -1;
	if ((*flag) == DECRYPT_FLAG) {
		if (getString(nArgs,args,keyBuffer,"-k",2) != 1)
			return ERR_FLAGS;
	}
	if (getInteger(nArgs,args,blockSize,"-s",2) != 1)
		return ERR_FLAGS;
	if (getString(nArgs,args,ivBuffer,"-iv",3) != 1)
		return ERR_FLAGS;
	if (getInteger(nArgs,args,ivLength,"-l",2) != 1)
		return ERR_FLAGS;
	if (getFormat(nArgs,args,inputFormat,"-if",3) != 1)
		return ERR_FLAGS;
	if (getFormat(nArgs,args,outputFormat,"-of",3) != 1)
		return ERR_FLAGS;

	if ((((*flag) == ENCRYPT_FLAG) && (nArgs == 8)) || (((*flag) == DECRYPT_FLAG) && (nArgs == 10))) {
		*flagInput = 0;
		*flagOutput = 0;
	}
	else {
		if (((*flagInput) = getString(nArgs,args,inputFileName,"-i",2)) == -1)
			return ERR_FLAGS;
		if (((*flagOutput) = getString(nArgs,args,outputFileName,"-o",2)) == -1)
			return ERR_FLAGS;
		if (((((*flag) == ENCRYPT_FLAG) && (nArgs == 14)) || (((*flag) == DECRYPT_FLAG) && (nArgs == 16))) && (*flagInput + *flagOutput != 1))
			return ERR_FLAGS;
		if (((((*flag) == ENCRYPT_FLAG) && (nArgs == 16)) || (((*flag) == DECRYPT_FLAG) && (nArgs == 18))) && (*flagInput + *flagOutput != 2))
			return ERR_FLAGS;
	}

	return 0;
}

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

void padding(char* text, int* textLength, int number, char padChar) {

	while ((*textLength)*8 % number != 0)
		text[(*textLength)++] = padChar;
}


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

void printKey(FILE* outputFile, DESblock* key) {
	printHexaBlock(outputFile,key,DES_KEY_SIZE,"Key = ");
}

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

void charToHex(char* hex, uint8_t c) {

	uint8_t bin[8];

	charToBin(bin,c);
	binToHex(hex,bin);
	binToHex(hex+1,bin+4);
}

int hexaToBlock(DESblock* newBlock, char* string, int length) {

	int i;
	
	if (strlen(string) != (length/4 + 2))
		return -1;

	if ((string[0] != '0') || (string[1] != 'x'))
		return -1;

	for (i=0; i<length/4; i++) {
		if (!isHex(string[i+2]))
			return -1;
		hexToBin(&(newBlock->block[4*i+1]),string[i+2]);
	}

	return 0;
}

int validKey(DESblock* key) {

	int byte, bit;
	int acc;

	for (byte=0; byte<DES_KEY_SIZE/8; byte++) {
		acc = 0;
		for (bit=0; bit<8; bit++)
			acc += key->block[8*byte+bit+1];
		if (acc%2 == 0)
			return 0;
	}

	return 1;
}

void newKey(DESblock* key) {

	int byte, bit;
	int acc;

	for (byte=0; byte<DES_KEY_SIZE/8; byte++) {
		acc = 0;
		for (bit=0; bit<7; bit++)
			acc += (key->block[8*byte+bit+1] = randomInt(0,1));
		key->block[8*byte+7+1] = (acc%2 == 0);
	}
}
void printHexaBlock(FILE* outputFile, DESblock* b, int blockSize, char* text) {

	char c;
	int i, j;
	uint8_t binBuffer[4];

	if (text)
		fprintf(outputFile,"%s",text);
	fprintf(outputFile,"%s","0x");

	for (i=0; i<blockSize/4; i++) {
		for (j=0; j<4; j++)
			binBuffer[j] = b->block[i*4+j+1];
		binToHex(&c,binBuffer);
		fputc(c,outputFile);
	}
	fputc('\n',outputFile);
}
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
void textToBlock(DESblock* b, char* text, int blockSize) {

	int i;
	int nBytes = blockSize/8;

	for (i=0; i<nBytes ; i++)
		charToBin(b->block + i*8 + 1, text[i]);
}

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

void blockToText(char* text, DESblock* b, int blockSize) {

	int i;
	int nBytes = blockSize/8;

	for (i=0; i<nBytes; i++)
		binToChar(((uint8_t*)text)+i,b->block + 8*i + 1);
}

void shiftRegister(DESblock* output, DESblock* input, int shift) {

	int i;

	for (i=1; i <= DES_BLOCK_SIZE-shift; i++)
		output->block[i] = output->block[i+shift];
	for (i=1; i <= shift; i++)
		output->block[i+DES_BLOCK_SIZE-shift] = input->block[i];
}

void charToBin(uint8_t* bin, uint8_t c) {

	int i;

	for (i=7; i>=0; i--) {
		bin[i] = c%2;
		c = c/2;
	}
}

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

void DES(DESblock* output, DESblock* input, DESblock* key, int flag) {

	DESblock oldLeft, oldRight, newLeft, newRight;
	DESblock ipBlock, rBlock, sBlock;
	DESblock oldKey, newKey, roundKey;
	int i;

	/* Initial permutation */
	initialPerm(&ipBlock,input);

	/* Splits the block into the two halves */
	leftSemiBlock(&oldLeft,&ipBlock);
	rightSemiBlock(&oldRight,&ipBlock);

	/* Permutation Choice 1 */
	permChoice1(&oldKey,key);

	/* NUM_ROUNDS rounds */
	for (i=1; i<= NUM_ROUNDS; i++) {

		LCS(&newKey,&oldKey,i,flag);
		permChoice2(&roundKey,&newKey);

		singleRound(&newLeft,&newRight,&oldLeft,&oldRight,&roundKey,i);

		copyBlock(&oldLeft,&newLeft,BITS_IN_FEISTEL/2);
		copyBlock(&oldRight,&newRight,BITS_IN_FEISTEL/2);
		copyBlock(&oldKey,&newKey,BITS_IN_PC1);
	}

	/* Merges the semiblocks */
	mergeSemiBlocks(&rBlock,&oldLeft,&oldRight);

	/* 32-bit swap */
	swap(&sBlock,&rBlock);

	/* Inverse permutation */
	invInitialPerm(output,&sBlock);
}

void xorDES(DESblock* new, DESblock* old1, DESblock* old2, int length) {

	int i;

	for (i=1; i <= length; i++) 
		new->block[i] = (old1->block[i] != old2->block[i]);
}

void binToChar(uint8_t* c, uint8_t* bin) {

	int i;
	int pow = 1;

	(*c) = 0;

	for (i=7; i>=0; i--) {
		(*c) += pow*bin[i];
		pow *= 2;
	}
}

void initialPerm(DESblock* new, DESblock* old) {
	selectDES(new, old, IP, BITS_IN_IP);
}

void leftSemiBlock(DESblock* semiBlock, DESblock* fullBlock) {

	int i;

	for (i=1; i <= BITS_IN_FEISTEL/2; i++)
		semiBlock->block[i] = fullBlock->block[i];
}


void rightSemiBlock(DESblock* semiBlock, DESblock* fullBlock) {

	int i;

	for (i=1; i <= BITS_IN_FEISTEL/2; i++)
		semiBlock->block[i] = fullBlock->block[i+BITS_IN_FEISTEL/2];
}

void permChoice1(DESblock* new, DESblock* old) {
	selectDES(new, old, PC1, BITS_IN_PC1);
}

void LCS(DESblock* new, DESblock* old, int nRound, int flag) {

	if (flag == ENCRYPT_FLAG)
		shiftLeftDES(new, old, ROUND_SHIFTS[nRound-1]);
	else if (flag == DECRYPT_FLAG)
		shiftRightDES(new, old, ROUND_SHIFTS_DEC[nRound-1]);
}

void permChoice2(DESblock* new, DESblock* old) {
	selectDES(new, old, PC2, BITS_IN_PC2);
}

void singleRound(DESblock* newLeft, DESblock* newRight, DESblock* oldLeft, DESblock* oldRight, DESblock* key, int nRound) {

	DESblock eBlock, xBlock, sBlock, pBlock;

	/* F function */
	expansion(&eBlock,oldRight);
	xorDES(&xBlock,&eBlock,key,BITS_IN_E);
	SBox(&sBlock,&xBlock);
	permutation(&pBlock,&sBlock);

	/* L(i) = R(i-1) */
	copyBlock(newLeft,oldRight,BITS_IN_FEISTEL/2);

	/* R(i) = L(i-1) xor F(R(i-1),k(i)) */
	xorDES(newRight,oldLeft,&pBlock,BITS_IN_P);
}

void copyBlock(DESblock* new, DESblock* old, int length) {

	int i;

	for (i=1; i <= length; i++)
		new->block[i] = old->block[i];
}

void mergeSemiBlocks(DESblock* fullBlock, DESblock* left, DESblock* right) {

	int i;

	for (i=1; i <= BITS_IN_FEISTEL/2; i++) {
		fullBlock->block[i] = left->block[i];
		fullBlock->block[BITS_IN_FEISTEL/2+i] = right->block[i];
	}
}

void invInitialPerm(DESblock* new, DESblock* old) {
	selectDES(new, old, IP_INV, BITS_IN_IP);
}

void selectDES(DESblock* new, DESblock* old, const unsigned short* indices, int length) {

	int i;

	for (i=1; i <= length; i++)
		new->block[i] = old->block[indices[i-1]];
}

void expansion(DESblock* new, DESblock* old) {
	selectDES(new, old, E, BITS_IN_E);
}

void SBox(DESblock* new, DESblock* old) {

	int i, row, col;
	int value;

	for (i=0; i < NUM_S_BOXES; i++) {
		row = 2*old->block[1+6*i] + old->block[1+6*i+5];
		col = 8*old->block[1+6*i+1] + 4*old->block[1+6*i+2] + 2*old->block[1+6*i+3] + old->block[1+6*i+4];
		
		value = S_BOXES[i][row][col];
		new->block[1+i*4+3] = value%2;
		new->block[1+i*4+2] = (value/2)%2;
		new->block[1+i*4+1] = (value/4)%2;
		new->block[1+i*4] = (value/8)%2;
	}
}

void permutation(DESblock* new, DESblock* old) {
	selectDES(new, old, P, BITS_IN_P);
}



