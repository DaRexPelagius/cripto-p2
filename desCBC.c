#include "desCBC.h"
#include "interface.h"
#include "DESmodes.h"


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

