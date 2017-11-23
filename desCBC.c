
#include "desCBC.h"


/*--------------------------------------------------------------------------
cbc
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

/* Shifts in each of the clave halves in each round (for encryption) */
static const unsigned short ROUND_SHIFTS[NUM_ROUNDS] = {
	1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

/* Shifts in each of the clave halves in each round (for decryption) */
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

	int flag, tamBloque, lenVecIni, entrada, salida;
	int lon;
	int aux;
	char formatoEntrada, formatoSalida;
	char clave[MAX_BUFFER];
	char bufferVecIni[MAX_BUFFER];
	char ficheroentrada[MAX_NOMBRE];
	char ficherosalida[MAX_NOMBRE];
	FILE* fentrada = NULL;
	FILE* fsalida = NULL;
	CBC cbc;
	char textoplano[MAX_TEXTO];
	char textocifrado[MAX_TEXTO];

	//Comprobamos que los argumentos son los correctos
	if ((argc != 12) && (argc != 14) && (argc != 16) && (argc != 18)){
		printf("Número de argumentos incorrecto.\n");
		printf("Uso del programa: ./1 {-P | -I} [-i filein] [-o fileout].\n");	
		return -1;
	}
	//Como en la practica anterior usamos una funcion auxiliar para conseguir los argumentos
	if (getArgs(argc, argv, &flag, clave, &tamBloque, bufferVecIni, &lenVecIni, &formatoEntrada, &formatoSalida, ficheroentrada, &entrada, ficherosalida, &salida) == -1) {
		printf("Argumentos incorrectos.\n");
		printf("Uso del programa: ./1 {-P | -I} [-i filein] [-o fileout].\n");
		return -1;
	}
	
	//Ahora vamos a comprar que los argumentos del DES sean correctos..

	//Tamanyo de bloque entre los limites
	if ((tamBloque < 1) || (tamBloque > TAM_BLOQUE)) {
		fprintf(stderr,"ERROR: Block size must be between 1 and %d\n",TAM_BLOQUE);
		return -1;
	}
	//Tamanyo de bloque multiplo de 8
	if ((tamBloque % 8) != 0) {
		fprintf(stderr,"ERROR: Block size must be a multiple of 8\n");
		return -1;
	}
	//Vector de inicializacion correcto
	if ((lenVecIni < 1) || (lenVecIni > tamBloque)) {
		fprintf(stderr,"ERROR: IV length must be between 1 and %d\n",tamBloque);
		return -1;
	}
	//Vector de inicializacion multiplo de 8
	if ((lenVecIni % 4) != 0) {
		fprintf(stderr,"ERROR: IV length must be a multiple of 4\n");
		return -1;
	}
	//Comprobamos que el archivo de entrada esta en un lenguaje que entendamos
	if ((formatoEntrada != FORMATO_HEX) && (formatoEntrada != FORMATO_ASCII)) {
		fprintf(stderr,"ERROR: Wrong input formato\n");
		return -1;
	}
	//Comprobamos que el lenguaje de salida lo conocemos
	if ((formatoSalida != FORMATO_HEX) && (formatoSalida != FORMATO_ASCII)) {
		fprintf(stderr,"ERROR: Wrong input formato\n");
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
	if (flag == 1) aux = prepararCBC(NULL, bufferVecIni, lenVecIni, tamBloque, &cbc);
	else aux = prepararCBC(clave, bufferVecIni, lenVecIni, tamBloque, &cbc);
	
	if (aux != 0) {
		if (fentrada) fclose(fentrada);
		if (fsalida) fclose(fsalida);
		return -1;
	}

	//Imprimimos la clave de Encriptacion 
	if (flag == 1) imprimirClave(stdout, &(cbc.clave));

	//Si el programa se ejecuto como encriptacion, encriptamos
	if (flag == 1) {
		if (!entrada) fprintf(stdout, "textoplano:\n");
		lon = leerEntrada(fentrada, formatoEntrada, textoplano, MAX_TEXTO);
		padding(textoplano, &lon, tamBloque, PADDING_CHAR);
		CFBmode(flag, textoplano, textocifrado, lon, &cbc);
		if (!salida) fprintf(stdout, "textocifrado:\n");
		imprimirSalida(fsalida, formatoSalida, textocifrado, lon);
	}

	//Si el programa se ejecuto como desencriptacion, desencriptamos
	else if (flag == 2) {
		if (!entrada) fprintf(stdout,"textocifrado:\n");
		lon = leerEntrada(fentrada,formatoEntrada,textocifrado,MAX_TEXTO);
		CFBmode(flag,textoplano,textocifrado,lon,&cbc);
		if (!salida) fprintf(stdout,"textoplano:\n");
		imprimirSalida(fsalida,formatoSalida,textoplano,lon);
	}

	if (fentrada) fclose(fentrada);
	if (fsalida) fclose(fsalida);

	return 0;
}

int getArgs(int nArgs, char** args, int* flag, char* clave, int* tamBloque, char* bufferVecIni, int* lenVecIni, char* formatoEntrada, char* formatoSalida, char* ficheroentrada, int* entrada, char* ficherosalida, int* salida) {

	if (getModo(nArgs, args, flag) != 1) return -1;
	if ((*flag) == 2) {
		if (getCadena(nArgs, args, clave, "-k", 2) != 1) return -1;
	}
	if (getEntero(nArgs, args, tamBloque, "-s", 2) != 1) return -1;
	if (getCadena(nArgs, args, bufferVecIni, "-iv", 3) != 1) return -1;
	if (getEntero(nArgs, args, lenVecIni, "-l", 2) != 1) return -1;
	if (getFormato(nArgs, args, formatoEntrada, "-if", 3) != 1) return -1;
	if (getFormato(nArgs, args, formatoSalida, "-of", 3) != 1) return -1;

	if ((((*flag) == 1) && (nArgs == 8)) || (((*flag) == 2) && (nArgs == 10))) {
		*entrada = 0;
		*salida = 0;
	}
	else {
		if (((*entrada) = getCadena(nArgs, args, ficheroentrada, "-i", 2)) == -1) return -1;
		if (((*salida) = getCadena(nArgs, args, ficherosalida, "-o", 2)) == -1) return -1;
		if (((((*flag) == 1) && (nArgs == 14)) || (((*flag) == 2) && (nArgs == 16))) && (*entrada + *salida != 1)) return -1;
		if (((((*flag) == 1) && (nArgs == 16)) || (((*flag) == 2) && (nArgs == 18))) && (*entrada + *salida != 2)) return -1;
	}

	return 0;
}
/*--------------------------------------------------------------------------
Obtiene un entero
- Entrada:
	* Número de argumentos
	* Array de argumentos
	* Puntero al entero
	* Puntero al modo
	* Longitud de la cadena
- Salida:
	* -1 si ocurre algun error
	* 1 si va bien
--------------------------------------------------------------------------*/
int getEntero(int nArgs, char** args, int* entero, char* modo, int longitud) {

	int i;
	int flag = 0;

	for (i=1; i <= (nArgs - 2); i++){
				
		if ((strncmp(args[i], modo, longitud) == 0) && (strlen(args[i]) == longitud)) {
			if (flag) return -1;
			else {
				*entero = atoi(args[i+1]);
				if ((*entero == 0) && !isdigit(args[i + 1][0])) return -1;
				flag = 1;
			}
		}}

	return flag;
}
/*--------------------------------------------------------------------------
Obtiene una cadena
- Entrada:
	* Número de argumentos
	* Array de argumentos
	* Puntero a la cadena
	* Puntero al modo
	* Longitud de la cadena
- Salida:
	* -1 si ocurre algun error
	* 1 si va bien
--------------------------------------------------------------------------*/
int getCadena(int nArgs, char** args, char* cadena, char* modo, int longitud) {

	int i;
	int flag = 0;

	for (i = 1; i <= (nArgs - 2); i++)
		if ((strncmp(args[i], modo, longitud) == 0) && (strlen(args[i]) == longitud)) {
			if (flag)
				return -1;
			else {
				strcpy(cadena, args[i+1]);
				flag = 1;
			}
		}

	return flag;
}
/*--------------------------------------------------------------------------
Obtiene el modo
- Entrada:
	* Número de argumentos
	* Array de argumentos
	* Puntero al modo
- Salida:
	* -1 si ocurre algun error
	* 1 si va bien
--------------------------------------------------------------------------*/
int getModo(int nArgs, char** args, int* modo) {

	int i;
	int flag = 0;

	for (i = 1; i < nArgs; i++){
		if ((strncmp(args[i], "-C", 2) == 0) && (strlen(args[i]) == 2)) {
			if (flag) return -1;
			else {
				*modo = 1;
				flag = 1;
			}
		}
		else if (strncmp(args[i],"-D",2) == 0) {
			if (flag) return -1;
			else {
				*modo = 2;
				flag = 1;
			}
		}
	}
	return flag;
}
/////////////////////FALTA CABECERA
int getFormato(int nArgs, char** args, char* formato, char* modo, int flagLength) {

	int i;
	int aux = 0;

	for (i=1; i<=nArgs-2; i++){
		if ((strncmp(args[i], modo, flagLength) == 0) && (strlen(args[i]) == flagLength)) {
			if (aux) return -1;
			else {
				if ((strlen(args[i + 1]) == 1) && (args[i + 1][0] == FORMATO_HEX)) {
					*formato = FORMATO_HEX;
					aux = 1;
				} else if ((strlen(args[i + 1]) == 1) && (args[i + 1][0] == FORMATO_BIN)) {
					*formato = FORMATO_BIN;
					aux = 1;
				} else if ((strlen(args[i + 1]) == 1) && (args[i + 1][0] == FORMATO_ASCII)) {
					*formato = FORMATO_ASCII;
					aux = 1;
				} else return -1;
			}
		}
	}
	return aux;
}

void imprimirSalida(FILE* fsalida, char formato, char* texto, int lon) {

	int i;
	char hex[2];
	//Si el formato es ASCII imprimimos tal cual
	if (formato == FORMATO_ASCII) for (i = 0; i < lon; i++) fputc(texto[i], fsalida);
	else if (formato == FORMATO_HEX) {//Sino transformamos los chars a digitos hexagonales
		for (i = 0; i < lon; i++) {
			char2Hex(hex, texto[i]);
			fputc(hex[0], fsalida);
			fputc(hex[1], fsalida);
		}
	}

	fputc('\n', fsalida);
}

void padding(char* texto, int* lon, int numero, char padChar) {

	while ((*lon) * 8 % numero != 0)	texto[(*lon)++] = padChar;
}


int prepararCBC(char* clave, char* bufferVecIni, int lenVecIni, int tamBloque, CBC* cbc) {

	//Inicializamos la clave y comprobamos que es valida
	if (clave) {
		if (hexaToBlock(&(cbc->clave),clave,TAM_CLAVE) == -1){
			fprintf(stderr,"ERROR: Error en el formato de la clave.\n");
			return -1;
		}else if (!esValida(&(cbc->clave))){
			fprintf(stderr,"ERROR: Clave no valida.\n");
			return -1;
		}
	} else {//Inicializamos una clave aleatoria
		srand(time(NULL));
		newClave(&(cbc->clave));
	}

	//Preparamos el vector de inicializacion
	if (hexaToBlock(&(cbc->IV),bufferVecIni,lenVecIni) == -1){
		fprintf(stderr,"ERROR: Error en el formato del vector de inicializacion.\n");		
		return -1;
	}
	cbc->lenVecIni = lenVecIni;

	//Guardamos el tamanyo de bloque
	cbc->tamBloque = tamBloque;

	return 0;
}

void imprimirClave(FILE* fsalida, Bloque* clave) {
	printHexaBlock(fsalida, clave, TAM_CLAVE, "Clave = ");
}

int leerEntrada(FILE* fentrada, char formato, char* texto, int maxLon) {

	char c;
	uint8_t auxC;
	char hex[2];
	int n = 0;

	if (formato == FORMATO_ASCII)
		while (((c = fgetc(fentrada)) != EOF) && (fentrada != stdin || c != '\n') && (n < maxLon)) {
			texto[n] = c;
			n++;
		}

	else if (formato == FORMATO_HEX)

		while (1) {

			/* Reads first half of the byte */
			hex[0] = fgetc(fentrada);
			while (!isHex(hex[0])) {
				/* End of input */
				if ((hex[0] == EOF) || ((fentrada == stdin) && (hex[0] == '\n')))
					return n;
				hex[0] = fgetc(fentrada);
			}

			/* Reads second half of the byte */
			hex[1] = fgetc(fentrada);
			while (!isHex(hex[1])) {
				/* End of input with an incomplete byte => Padding */
				if ((hex[1] == EOF) || ((fentrada == stdin) && (hex[1] == '\n'))) {
					hex[1] = '0';
					hexToChar(&auxC,hex);
					texto[n] = auxC;
					n++;
					return n;
				}
				hex[1] = fgetc(fentrada);
			}

			/* Translates the complete byte */
			hexToChar(&auxC,hex);
			texto[n] = auxC;
			n++;
		}

	return n;
}

void CFBmode(int flag, char* textoplano, char* textocifrado, int lon, CBC* cbc) {
	
	Bloque plainBlock, cipherBlock;
	int i;
	int l = cbc->lenVecIni;
	int s = cbc->tamBloque;
	int nBlocks = lon * 8 / s;

	/* Initial shift register content */
	for (i=1; i<=TAM_BLOQUE-l; i++)
		cbc->SR.block[i] = 0;
	for (i=1; i<=l; i++)
		cbc->SR.block[TAM_BLOQUE-l+i] = cbc->IV.block[i];

	/* Encryption/decryption by blocks */
	if (flag == 1) {
		for (i=0; i<nBlocks; i++) {
			textToBlock(&plainBlock, textoplano + i*s/8, s);
			CFBstep(flag,&plainBlock,&cipherBlock,cbc);
			blockToText(textocifrado + i*s/8, &cipherBlock, s);
		}
	}
	else {
		for (i=0; i < nBlocks; i++) {
			textToBlock(&cipherBlock, textocifrado + i*s/8, s);
			CFBstep(flag,&plainBlock,&cipherBlock,cbc);
			blockToText(textoplano + i*s/8, &plainBlock, s);
		}
	}
}

void char2Hex(char* hex, uint8_t c) {

	uint8_t bin[8];

	charToBin(bin,c);
	binToHex(hex,bin);
	binToHex(hex+1,bin+4);
}

int hexaToBlock(Bloque* newBlock, char* string, int length) {

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

int esValida(Bloque* clave) {

	int byte, bit;
	int acc;

	for (byte = 0; byte < TAM_CLAVE / 8; byte++) {
		acc = 0;
		for (bit = 0; bit < 8; bit++) acc += clave->block[8 * byte + bit + 1];
		if (acc % 2 == 0) return 0;
	}

	return 1;
}

void newClave(Bloque* clave) {

	int byte, bit;
	int acc;

	for (byte=0; byte<TAM_CLAVE/8; byte++) {
		acc = 0;
		for (bit=0; bit<7; bit++) acc += (clave->block[8*byte+bit+1] = naleatorio(0,1));
		clave->block[8*byte+7+1] = (acc%2 == 0);
	}
}

int naleatorio(int a, int b) {

	if (a >= b) return a;

	return a + (rand() % (b - a + 1));
}

void printHexaBlock(FILE* fsalida, Bloque* b, int tamBloque, char* texto) {

	char c;
	int i, j;
	uint8_t binBuffer[4];

	if (texto)
		fprintf(fsalida,"%s",texto);
	fprintf(fsalida,"%s","0x");

	for (i=0; i<tamBloque/4; i++) {
		for (j=0; j<4; j++)
			binBuffer[j] = b->block[i*4+j+1];
		binToHex(&c,binBuffer);
		fputc(c,fsalida);
	}
	fputc('\n',fsalida);
}
// isHex @ number.c
int isHex(char hex) {

	if (((hex >= '0') && (hex <= '9')) || ((hex >= 'A') && (hex <= 'F')) || ((hex >= 'a') && (hex <= 'f')))
		return 1;

	return 0;
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
void textToBlock(Bloque* b, char* texto, int tamBloque) {

	int i;
	int nBytes = tamBloque/8;

	for (i=0; i<nBytes ; i++)
		charToBin(b->block + i*8 + 1, texto[i]);
}

void CFBstep(int flag, Bloque* plainBlock, Bloque* cipherBlock, CBC* cbc) {

	int s = cbc->tamBloque;
	Bloque outputDES;

	/* DES algorithm (input from shift register) */
	DES(&outputDES,&(cbc->SR),&(cbc->clave),1);

	/* XOR with the s most significant bits */
	if (flag == 1)
		xorDES(cipherBlock,plainBlock,&outputDES,s);
	else
		xorDES(plainBlock,cipherBlock,&outputDES,s);

	/* Shift register ready for the next block */
	shiftRegister(&(cbc->SR),cipherBlock,s);
}

void blockToText(char* texto, Bloque* b, int tamBloque) {

	int i;
	int nBytes = tamBloque/8;

	for (i=0; i<nBytes; i++)
		binToChar(((uint8_t*)texto)+i,b->block + 8*i + 1);
}

void shiftRegister(Bloque* output, Bloque* input, int shift) {

	int i;

	for (i=1; i <= TAM_BLOQUE-shift; i++)
		output->block[i] = output->block[i+shift];
	for (i=1; i <= shift; i++)
		output->block[i+TAM_BLOQUE-shift] = input->block[i];
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

void DES(Bloque* output, Bloque* input, Bloque* clave, int flag) {

	Bloque oldLeft, oldRight, newLeft, newRight;
	Bloque ipBlock, rBlock, sBlock;
	Bloque oldKey, newClave, roundKey;
	int i;

	/* Initial permutation */
	initialPerm(&ipBlock,input);

	/* Splits the block into the two halves */
	leftSemiBlock(&oldLeft,&ipBlock);
	rightSemiBlock(&oldRight,&ipBlock);

	/* Permutation Choice 1 */
	permChoice1(&oldKey,clave);

	/* NUM_ROUNDS rounds */
	for (i=1; i<= NUM_ROUNDS; i++) {

		LCS(&newClave,&oldKey,i,flag);
		permChoice2(&roundKey,&newClave);

		singleRound(&newLeft,&newRight,&oldLeft,&oldRight,&roundKey,i);

		copyBlock(&oldLeft,&newLeft,BITS_IN_FEISTEL/2);
		copyBlock(&oldRight,&newRight,BITS_IN_FEISTEL/2);
		copyBlock(&oldKey,&newClave,BITS_IN_PC1);
	}

	/* Merges the semiblocks */
	mergeSemiBlocks(&rBlock,&oldLeft,&oldRight);

	/* 32-bit swap */
	swap(&sBlock,&rBlock);

	/* Inverse permutation */
	invInitialPerm(output,&sBlock);
}

void xorDES(Bloque* new, Bloque* old1, Bloque* old2, int length) {

	int i;

	for (i=1; i <= length; i++) 
		new->block[i] = (old1->block[i] != old2->block[i]);
}

void binToChar(uint8_t* c, uint8_t* bin) {

	int i;
	int pow = 1;

	(*c) = 0;

	for (i = 7; i >= 0; i--) {
		(*c) += pow * bin[i];
		pow *= 2;
	}
}

void initialPerm(Bloque* new, Bloque* old) {
	selectDES(new, old, IP, BITS_IN_IP);
}

void leftSemiBlock(Bloque* semiBlock, Bloque* fullBlock) {

	int i;

	for (i=1; i <= BITS_IN_FEISTEL/2; i++)
		semiBlock->block[i] = fullBlock->block[i];
}


void rightSemiBlock(Bloque* semiBlock, Bloque* fullBlock) {

	int i;

	for (i=1; i <= BITS_IN_FEISTEL/2; i++)
		semiBlock->block[i] = fullBlock->block[i+BITS_IN_FEISTEL/2];
}

void permChoice1(Bloque* new, Bloque* old) {
	selectDES(new, old, PC1, BITS_IN_PC1);
}

void LCS(Bloque* new, Bloque* old, int nRound, int flag) {

	if (flag == 1) shiftLeftDES(new, old, ROUND_SHIFTS[nRound-1]);
	else if (flag == 2) shiftRightDES(new, old, ROUND_SHIFTS_DEC[nRound-1]);
}

//ShiftLeftDES @ Des.c
void shiftLeftDES(Bloque* new, Bloque* old, int shift) {

	int i;

	for (i=0; i < BITS_MEDIOBLOQUE; i++) new->block[i+1] = old->block[((i+shift)%(BITS_MEDIOBLOQUE)) + 1];
	for (i=0; i < BITS_MEDIOBLOQUE; i++) new->block[i+BITS_MEDIOBLOQUE+1] = old->block[((i+shift)%(BITS_MEDIOBLOQUE)) + BITS_MEDIOBLOQUE + 1];
}
//ShiftRightDES @ Des.c
void shiftRightDES(Bloque* new, Bloque* old, int shift) {

	int i;

	for (i=0; i < BITS_MEDIOBLOQUE; i++) new->block[i+1] = old->block[((i-shift+BITS_MEDIOBLOQUE)%(BITS_MEDIOBLOQUE)) + 1];
	for (i=0; i < BITS_MEDIOBLOQUE; i++) new->block[i+BITS_MEDIOBLOQUE+1] = old->block[((i-shift+BITS_MEDIOBLOQUE)%(BITS_MEDIOBLOQUE)) + BITS_MEDIOBLOQUE + 1];
}

void permChoice2(Bloque* new, Bloque* old) {
	selectDES(new, old, PC2, BITS_IN_PC2);
}

void singleRound(Bloque* newLeft, Bloque* newRight, Bloque* oldLeft, Bloque* oldRight, Bloque* clave, int nRound) {

	Bloque eBlock, xBlock, sBlock, pBlock;

	/* F function */
	expansion(&eBlock,oldRight);
	xorDES(&xBlock,&eBlock,clave,BITS_IN_E);
	SBox(&sBlock,&xBlock);
	permutation(&pBlock,&sBlock);

	/* L(i) = R(i-1) */
	copyBlock(newLeft,oldRight,BITS_IN_FEISTEL/2);

	/* R(i) = L(i-1) xor F(R(i-1),k(i)) */
	xorDES(newRight,oldLeft,&pBlock,BITS_IN_P);
}

void copyBlock(Bloque* new, Bloque* old, int length) {

	int i;

	for (i=1; i <= length; i++)
		new->block[i] = old->block[i];
}

void mergeSemiBlocks(Bloque* fullBlock, Bloque* left, Bloque* right) {

	int i;

	for (i=1; i <= BITS_IN_FEISTEL/2; i++) {
		fullBlock->block[i] = left->block[i];
		fullBlock->block[BITS_IN_FEISTEL/2+i] = right->block[i];
	}
}

void swap(Bloque* new, Bloque* old) {
	selectDES(new, old, SWAP, BITS_IN_SWAP);
}

void invInitialPerm(Bloque* new, Bloque* old) {
	selectDES(new, old, IP_INV, BITS_IN_IP);
}

void selectDES(Bloque* new, Bloque* old, const unsigned short* indices, int length) {

	int i;

	for (i=1; i <= length; i++)
		new->block[i] = old->block[indices[i-1]];
}

void expansion(Bloque* new, Bloque* old) {
	selectDES(new, old, E, BITS_IN_E);
}

void SBox(Bloque* new, Bloque* old) {

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

void permutation(Bloque* new, Bloque* old) {
	selectDES(new, old, P, BITS_IN_P);
}




