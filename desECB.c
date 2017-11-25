
#include "desECB.h"


/*--------------------------------------------------------------------------
ecb
--------------------------------------------------------------------------*/
/* PC1 "permutacionF_DES" */
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

/* PC2 "permutacionF_DES" */
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
static const unsigned short ROUND_SHIFTS[RONDAS] = {
	1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

/* Shifts in each of the clave halves in each round (for decryption) */
static const unsigned short ROUND_SHIFTS_DEC[RONDAS] = {
	0, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

/* IP permutacionF_DES */
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

/* P permutacionF_DES */
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
	ECB ecb;
	char textoplano[MAX_TEXTO];
	char textocifrado[MAX_TEXTO];

	//Comprobamos que los argumentos son los correctos
	if ((argc != 12) && (argc != 14) && (argc != 16) && (argc != 18)){
		printf("Número de argumentos incorrecto.\n");
		printf("Uso del programa: ./desECB {-P | -I} [-i filein] [-o fileout].\n");	
		return -1;
	}
	//Como en la practica anterior usamos una funcion auxiliar para conseguir los argumentos
	if (getArgs(argc, argv, &flag, clave, &tamBloque, bufferVecIni, &lenVecIni, &formatoEntrada, &formatoSalida, ficheroentrada, &entrada, ficherosalida, &salida) == -1) {
		printf("Argumentos incorrectos.\n");
		printf("Uso del programa: ./desECB {-P | -I} [-i filein] [-o fileout].\n");
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
	if (flag == 1) aux = prepararECB(NULL, tamBloque, &ecb);
	else aux = prepararECB(clave, tamBloque, &ecb);
	
	//Si nos da error cerramos los ficheros
	if (aux != 0) {
		if (fentrada) fclose(fentrada);
		if (fsalida) fclose(fsalida);
		return -1;
	}

	//Imprimimos la clave de Encriptacion 
	if (flag == 1) imprimirClave(stdout, &(ecb.clave));

	//Si el programa se ejecuto como encriptacion, encriptamos
	if (flag == 1) {
		if (!entrada) fprintf(stdout, "Texto plano:\n");
		lon = leerEntrada(fentrada, formatoEntrada, textoplano, MAX_TEXTO);
		padding(textoplano, &lon, tamBloque, PADDING_CHAR);
		modoECB(flag, textoplano, textocifrado, lon, &ecb);
		if (!salida) fprintf(stdout, "Texto cifrado:\n");
		imprimirSalida(fsalida, formatoSalida, textocifrado, lon);
	}
	//Si el programa se ejecuto como desencriptacion, desencriptamos
	else if (flag == 2) {
		if (!entrada) fprintf(stdout,"textocifrado:\n");
		lon = leerEntrada(fentrada, formatoEntrada, textocifrado, MAX_TEXTO);
		modoECB(flag, textoplano, textocifrado, lon, &ecb);
		if (!salida) fprintf(stdout, "textoplano:\n");
		imprimirSalida(fsalida, formatoSalida, textoplano, lon);
	}

	if (fentrada) fclose(fentrada);
	if (fsalida) fclose(fsalida);

	return 0;
}
/*--------------------------------------------------------------------------
Obtiene los argumentos
- Entrada:
	* Número de argumentos
	* Array de argumentos
	* Flag indicando si se cifra o descifra
	* Clave
	* Tamaño del bloque
	* IV
	* Longitud del IV
	* Formato de entrada
	* Formato de salida
	* Nombre del fichero de entrada
	* Puntero indicando si hay fichero de entrada
	* Nombre del fichero de salida
	* Puntero indicando si hay fichero de salida
- Salida:
	* -1 si ocurre algun error
	* 0 si va bien
--------------------------------------------------------------------------*/
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
/*--------------------------------------------------------------------------
Obtiene el formato
- Entrada:
	* Número de argumentos
	* Array de argumentos
	* Formato
	* Modo
	* Longitud del parametro
- Salida:
	* -1 si ocurre algun error
	* 0 si va bien
--------------------------------------------------------------------------*/
int getFormato(int nArgs, char** args, char* formato, char* modo, int lonParam) {

	int i;
	int aux = 0;

	for (i = 1; i <= nArgs - 2; i++){
		if ((strncmp(args[i], modo, lonParam) == 0) && (strlen(args[i]) == lonParam)) {
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
/*--------------------------------------------------------------------------
Imprime la salida
- Entrada:
	* Fichero donde imprimir
	* Texto a imprimir
	* Formato
	* Longitud
--------------------------------------------------------------------------*/
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
/*--------------------------------------------------------------------------
Realiza el padding
- Entrada:
	* Texto
	* Longitud
	* Numero que queremos que la longitud sea múltiplo de
	* Caracter con el que realizar el padding
--------------------------------------------------------------------------*/
void padding(char* texto, int* lon, int numero, char padChar) {
	while ((*lon) * 8 % numero != 0) texto[(*lon)++] = padChar;
}

/*--------------------------------------------------------------------------
Prepara el ECB, inicializando y comprobando la validez de la clave
- Entrada:
	* Clave
	* Tamaño del bloque
	* Puntero a la estructura de ECB
- Salida:
	* -1 si ocurre algun error
	* 0 si va bien
--------------------------------------------------------------------------*/
int prepararECB(char* clave, int tamBloque, ECB* ecb) {

	//Inicializamos la clave y comprobamos que es valida
	if (clave) {
		if (hex2Bloque(&(ecb->clave), clave, TAM_CLAVE) == -1){
			fprintf(stderr,"ERROR: Error en el formato de la clave.\n");
			return -1;
		}else if (!esValida(&(ecb->clave))){
			fprintf(stderr,"ERROR: Clave no valida.\n");
			return -1;
		}
	} else {//Inicializamos una clave aleatoria
		srand(time(NULL));
		newClave(&(ecb->clave));
	}

	//Guardamos el tamanyo de bloque
	ecb->tamBloque = tamBloque;

	return 0;
}
/*--------------------------------------------------------------------------
Imprime la clave
- Entrada:
	* Fichero de salida
	* Clave a imprimir
--------------------------------------------------------------------------*/
void imprimirClave(FILE* fsalida, Bloque* clave) {
	imprimirBloqueHexadecimal(fsalida, clave, TAM_CLAVE, "Clave = ");
}

/*--------------------------------------------------------------------------
Leer la entrada
- Entrada:
	* Fichero de entrada
	* Formato
	* Texto a leer
	* Maxima longitud
- Salida:
	* Longitud n
--------------------------------------------------------------------------*/
int leerEntrada(FILE* fentrada, char formato, char* texto, int maxLon) {

	char c;
	uint8_t auxC;
	char hex[2];
	int n = 0;
	
	//Si el formato de entrada es ASCII leemos tal cual
	if (formato == FORMATO_ASCII){
		while (((c = fgetc(fentrada)) != EOF) && (fentrada != stdin || c != '\n') && (n < maxLon)) {
			texto[n] = c;
			n++;
		}
	} else if (formato == FORMATO_HEX){
		while (1) {
			//Como un char es un byte, al estar en hexadecimal necesitamos
			//dos char, leemos el primero
			hex[0] = fgetc(fentrada);
			while (!comprobarHex(hex[0])) {//Comprobamos que la entrada es hexadecimal
				//Comprobamos si ha acabado la entrada, EOF si leemos de un fichero, \n de consola
				if ((hex[0] == EOF) || ((fentrada == stdin) && (hex[0] == '\n'))) return n;
				hex[0] = fgetc(fentrada);
			}

			//Cuando encontremos el primer digito hex, leemos su segundo byte
			hex[1] = fgetc(fentrada);
			while (!comprobarHex(hex[1])) {
				//Comprobamos si ha acabado la entrada, EOF si leemos de un fichero, \n de consola
				//Si acaba el fichero tenemos que completar(padding) el digito hexadecimal
				if ((hex[1] == EOF) || ((fentrada == stdin) && (hex[1] == '\n'))) {
					hex[1] = '0';
					hex2Char(&auxC, hex);
					texto[n] = auxC;
					n++;
					return n;
				}
				hex[1] = fgetc(fentrada);
			}

			//Traducimos el digito hexadecimal
			hex2Char(&auxC,hex);
			texto[n] = auxC;
			n++;
		}
	}
	return n;
}

/*--------------------------------------------------------------------------
Realiza el cifrado o descifrado siguiendo el metodo ECB
- Entrada:
	* Modo (cifrado/descifrado)
	* Texto plano
	* Texto cifrado
	* Longitud
	* Puntero a la estructura ECB
--------------------------------------------------------------------------*/
void modoECB(int modo, char* textoplano, char* textocifrado, int lon, ECB* ecb) {
	
	Bloque bloqueTextoPlano, bloqueTextoCifrado;
	int i;
	int tam = ecb->tamBloque; 
	int nBloques = lon * 8 / tam;

	//Segun el modo procedemos a cifrar o descifrar
	//Cabe decir que ciframos sobre los bits asi que tenemos que pasar
	//el texto a binario
	if (modo == 1) {
		for (i = 0; i < nBloques; i++) {
			texto2Bloque(&bloqueTextoPlano, textoplano + i * tam / 8, tam);
			fECB(modo, &bloqueTextoPlano, &bloqueTextoCifrado, ecb);
			bloque2Texto(textocifrado + i * tam / 8, &bloqueTextoCifrado, tam);
		}
	} else {
		for (i=0; i < nBloques; i++) {
			texto2Bloque(&bloqueTextoCifrado, textocifrado + i * tam / 8, tam);
			fECB(modo, &bloqueTextoPlano, &bloqueTextoCifrado, ecb);
			bloque2Texto(textoplano + i * tam / 8, &bloqueTextoPlano, tam);
		}
	}
}
/*--------------------------------------------------------------------------
Cambia un caracter a un hexadecimal
- Entrada:
	* Hexadicimal
	* Caracter
--------------------------------------------------------------------------*/
void char2Hex(char* hex, uint8_t c) {

	uint8_t bin[8];

	char2Bin(bin,c);
	bin2Hex(hex,bin);
	bin2Hex(hex+1,bin+4);
}
/*--------------------------------------------------------------------------
Cambia un hexadecimal a un bloque
- Entrada:
	* Puntero al Bloque
	* Cadena
	* Longitud
- Salida:
	* -1 si ocurre algun error
	* 0 si va bien
--------------------------------------------------------------------------*/
int hex2Bloque(Bloque* resultado, char* string, int length) {

	int i;
	
	if (strlen(string) != (length/4 + 2)) return -1;

	if ((string[0] != '0') || (string[1] != 'x')) return -1;

	for (i = 0; i < length / 4; i++) {
		if (!comprobarHex(string[i + 2])) return -1;
		hex2Bin(&(resultado->bloque[4 * i + 1]),string[i + 2]);
	}

	return 0;
}
/*--------------------------------------------------------------------------
Comprueba si una clave es valida
- Entrada:
	* Clave
- Salida:
	* 1 si la clave es valida
	* 0 si no
--------------------------------------------------------------------------*/
int esValida(Bloque* clave) {

	int byte, bit;
	int acc;

	for (byte = 0; byte < TAM_CLAVE / 8; byte++) {
		acc = 0;
		for (bit = 0; bit < 8; bit++) acc += clave->bloque[8 * byte + bit + 1];
		if (acc % 2 == 0) return 0;
	}

	return 1;
}
/*--------------------------------------------------------------------------
Genera una nueva clave
- Entrada:
	* Clave a generar
--------------------------------------------------------------------------*/
void newClave(Bloque* clave) {

	int byte, bit;
	int acc;

	for (byte = 0; byte < (TAM_CLAVE / 8); byte++) {
		acc = 0;
		for (bit = 0; bit < 7; bit++) acc += (clave->bloque[8 * byte + bit + 1] = naleatorio(0,1));
		clave->bloque[8 * byte + 7 + 1] = (acc % 2 == 0);
	}
}
/*--------------------------------------------------------------------------
Genera un numero aleatorio en el intervalo (a,b)
- Entrada:
	* Intervalo (a,b)
- Salida:
	* Numero generado
--------------------------------------------------------------------------*/
int naleatorio(int a, int b) {

	if (a >= b) return a;

	return a + (rand() % (b - a + 1));
}
/*--------------------------------------------------------------------------
Imprime un bloque
- Entrada:
	* Fichero de salida
	* Bloque
	* Tamaño del bloque
	* Texto
--------------------------------------------------------------------------*/
void imprimirBloqueHexadecimal(FILE* fsalida, Bloque* b, int tamBloque, char* texto) {

	char c;
	int i, j;
	uint8_t binBuffer[4];

	//Si existe texto de salida lo imprimimos
	if (texto) fprintf(fsalida, "%s", texto);
	//Sino imprimimos 0
	fprintf(fsalida,"%s","0x");
	
	//Imprimimos el bloque hexadecimal
	for (i = 0; i < tamBloque / 4; i++) {
		for (j = 0; j < 4; j++) binBuffer[j] = b->bloque[i * 4 + j + 1];
		bin2Hex(&c, binBuffer);
		fputc(c, fsalida);
	}
	fputc('\n', fsalida);//Terminamos con un /n para que no nos quede feo
}
/*--------------------------------------------------------------------------
Comprueba si un caracter es hexadecimal
- Entrada:
	* Caracter
- Salida:
	* 1 si es hexadecimal
	* 0 si no
--------------------------------------------------------------------------*/
int comprobarHex(char hex) {
	//Miramos que solo tiene numeros del 0-9 o letras A-F como tiene que ser en hexadecimal
	if (((hex >= '0') && (hex <= '9')) || ((hex >= 'A') && (hex <= 'F')) || ((hex >= 'a') && (hex <= 'f')))	return 1;

	return 0;
}
/*--------------------------------------------------------------------------
Paso de hexadecimal a caracter
- Entrada:
	* Caracter c
	* Hexadecimal
--------------------------------------------------------------------------*/
void hex2Char(uint8_t* c, char* hex) {

	//Hex tiene dos posiciones hex[0] y hex[1]
	//Siendo hex[0] los bits mas significativos
	if ((hex[1] >= '0') && (hex[1] <= '9'))	(*c) = hex[1] - '0';
	else if ((hex[1] >= 'A') && (hex[1] <= 'F')) (*c) = hex[1] + 10 - 'A'; //Ej: B-A = 1, 1+10=11=0Bx
	else if ((hex[1] >= 'a') && (hex[1] <= 'f')) (*c) = hex[1] + 10 - 'a';

	//Repetimos operacion con los bits mas significativos
	if ((hex[0] >= '0') && (hex[0] <= '9'))	(*c) += 16*(hex[0] - '0');
	else if ((hex[0] >= 'A') && (hex[0] <= 'F')) (*c) += 16*(hex[0] + 10 - 'A');
	else if ((hex[0] >= 'a') && (hex[0] <= 'f')) (*c) += 16*(hex[0] + 10 - 'a');

}
/*--------------------------------------------------------------------------
Paso de texto a bloque
- Entrada:
	* Bloque b
	* Texto
	* Tamaño del bloque
--------------------------------------------------------------------------*/
void texto2Bloque(Bloque* b, char* texto, int tamBloque) {

	int i;
	int nBytes = tamBloque / 8;

	for (i = 0; i < nBytes ; i++) char2Bin(b->bloque + i * 8 + 1, texto[i]);
}
/*--------------------------------------------------------------------------
Funcion que aplica ECB, como ECB es la aplicacion directa de DES, simplemente llamamos a DES
- Entrada:
	* Texto plano
	* Texto cifrado
	* Puntero a la estructura ECB
	* Flag
--------------------------------------------------------------------------*/
void fECB(int flag, Bloque* bloqueTextoPlano, Bloque* bloqueTextoCifrado, ECB* ecb) {

	//Aplicamos DES
	DES(bloqueTextoCifrado, bloqueTextoPlano, &(ecb->clave), 1);

}
/*--------------------------------------------------------------------------
Realiza la operación del DES de acuerdo al esquema visto en la teoria
- Entrada:
	* Entrada
	* Resultado
	* Modo
	* Clave
--------------------------------------------------------------------------*/
void DES(Bloque* resultado, Bloque* entrada, Bloque* clave, int modo) {

	Bloque izqEntrada, derEntrada, izqSalida, derSalida;
	Bloque bloquePermIni, bloqueTrasRondas, bloqueTrasSwap;
	Bloque claveEntrada, claveSalida, claveRonda;
	int i;

	//Permutamos la primera vez(Izq a Der y viceversa)
	permArranque(&bloquePermIni, entrada);

	//Dividimos en dos el bloque
	getMitadIzq(&izqEntrada, &bloquePermIni);
	getMitadDer(&derEntrada, &bloquePermIni);

	//Realizamos la primera permutacion del esquema a la clave. PC1
	permutacionClave1(&claveEntrada, clave);

	//Procedemos ahora con las 16 rondas de DES
	for (i = 1; i<= RONDAS; i++) {

		LCS(&claveSalida, &claveEntrada, i, modo);//Left Circular Shift
		permutacionClave2(&claveRonda, &claveSalida);

		rondaDES(&izqSalida, &derSalida, &izqEntrada, &derEntrada, &claveRonda);

		copiarBloque(&izqEntrada, &izqSalida, BITS_IN_FEISTEL/2);
		copiarBloque(&derEntrada, &derSalida, BITS_IN_FEISTEL/2);
		copiarBloque(&claveEntrada, &claveSalida, BITS_IN_PC1);
	}

	//Unimos los bloques
	unirBloques(&bloqueTrasRondas, &izqEntrada, &derEntrada);

	//Swap final de DES(32 bitd)
	swap(&bloqueTrasSwap, &bloqueTrasRondas);

	//IP^⁽⁻¹⁾
	invPermArranque(resultado, &bloqueTrasSwap);
}
//Left Circular Shift
void LCS(Bloque* resultado, Bloque* entrada, int nRonda, int modo) {

	if (modo == 1) shiftLeftDES(resultado, entrada, ROUND_SHIFTS[nRonda - 1]);
	else if (modo == 2) shiftRightDES(resultado, entrada, ROUND_SHIFTS_DEC[nRonda - 1]);
}

//ShiftLeftDES @ Des.c
void shiftLeftDES(Bloque* resultado, Bloque* entrada, int cambio) {

	int i;

	for (i = 0; i < BITS_MEDIOBLOQUE; i++) 
		resultado->bloque[i + 1] = entrada->bloque[((i + cambio) % (BITS_MEDIOBLOQUE)) + 1];
	for (i=0; i < BITS_MEDIOBLOQUE; i++) resultado->bloque[i + BITS_MEDIOBLOQUE + 1] = entrada->bloque[((i + cambio) % (BITS_MEDIOBLOQUE)) 
												+ BITS_MEDIOBLOQUE + 1];
}
//ShiftRightDES @ Des.c
void shiftRightDES(Bloque* resultado, Bloque* entrada, int cambio) {

	int i;

	for (i = 0; i < BITS_MEDIOBLOQUE; i++)
		resultado->bloque[i+1] = entrada->bloque[((i - cambio + BITS_MEDIOBLOQUE) % (BITS_MEDIOBLOQUE)) + 1];
	for (i=0; i < BITS_MEDIOBLOQUE; i++) 
		resultado->bloque[i + BITS_MEDIOBLOQUE + 1] = entrada->bloque[((i - cambio + BITS_MEDIOBLOQUE) % (BITS_MEDIOBLOQUE)) 
								+ BITS_MEDIOBLOQUE + 1];
}

//Permutacion con los valores que nos dan en el enunciado
//Initial permutacionF_DES.
void permArranque(Bloque* resultado, Bloque* entrada) {
	permutacion(resultado, entrada, IP, BITS_IN_IP);
}

//Permuta los valores del bloque
void permutacion(Bloque* resultado, Bloque* entrada, const unsigned short* indices, int lon) {

	int i;

	for (i = 1; i <= lon; i++) resultado->bloque[i] = entrada->bloque[indices[i - 1]];
}



void bloque2Texto(char* texto, Bloque* b, int tamBloque) {

	int i;
	int nBytes = tamBloque / 8;

	for (i=0; i < nBytes; i++) bin2Char(((uint8_t*) texto) + i, b->bloque + 8 * i + 1);
}


void char2Bin(uint8_t* bin, uint8_t c) {

	int i;
	
	//El paso a binario es sencillo
	for (i = 7; i >= 0; i--) {
		bin[i] = c % 2;
		c = c / 2;
	}
}

void bin2Hex(char* c, uint8_t* bin) {

	int i;
	int valor = 0;
	int potencia = 1;//2 elevado a |i-3|

	for (i = 3; i >= 0; i--) {
		valor += potencia * bin[i];
		potencia *= 2;
	}
	//Acabamos con un padding
	if (valor < 10)	(*c) = '0' + valor;
	else (*c) = 'A' + valor - 10;
}

void hex2Bin(uint8_t* bin, char hex) {

	int i, valorDecimal = 0;

	if ((hex >= '0') && (hex <= '9')) valorDecimal = hex - '0';
	else if ((hex >= 'A') && (hex <= 'Z')) valorDecimal = hex + 10 - 'A';
	else if ((hex >= 'a') && (hex <= 'z')) valorDecimal = hex + 10 - 'a';

	for (i = 3; i >= 0; i--) {
		bin[i] = valorDecimal % 2;
		valorDecimal = valorDecimal / 2;
	}
}


void xorDES(Bloque* resultado, Bloque* entrada1, Bloque* entrada2, int length) {

	int i;

	for (i = 1; i <= length; i++) 
		resultado->bloque[i] = (entrada1->bloque[i] != entrada2->bloque[i]);
}

void bin2Char(uint8_t* c, uint8_t* bin) {

	int i;
	int pow = 1;

	(*c) = 0;

	for (i = 7; i >= 0; i--) {
		(*c) += pow * bin[i];
		pow *= 2;
	}
}


void getMitadIzq(Bloque* resultado, Bloque* entrada) {

	int i;
	//Cogemos los 32 primeros bits(1-32), recordamos que en bloque->bloque[0] no hay nada util
	for (i = 1; i <= BITS_IN_FEISTEL / 2; i++) resultado->bloque[i] = entrada->bloque[i];
}


void getMitadDer(Bloque* resultado, Bloque* entrada) {

	int i;

	//Cogemos los 32 ultimos bits(33-64), recordamos que en bloque->bloque[0] no hay nada util
	for (i = 1; i <= BITS_IN_FEISTEL / 2; i++) resultado->bloque[i] = entrada->bloque[i + BITS_IN_FEISTEL / 2];
}

void permutacionClave1(Bloque* resultado, Bloque* entrada) {
	permutacion(resultado, entrada, PC1, BITS_IN_PC1);
}




void permutacionClave2(Bloque* resultado, Bloque* entrada) {
	permutacion(resultado, entrada, PC2, BITS_IN_PC2);
}

void rondaDES(Bloque* izqSalida, Bloque* derSalida, Bloque* izqEntrada, Bloque* derEntrada, Bloque* clave) {

	Bloque eBlock, xBlock, bloqueTrasSBOX, pBlock;

	//Aplicamos la funcion F de DES
	expansion(&eBlock, derEntrada); //La mitad derecha la preparamos para sumar con la clave, 48bits
	xorDES(&xBlock, &eBlock, clave, BITS_IN_E); //Hacemos xor de esa mitad derecha con la clave
	SBox(&bloqueTrasSBOX, &xBlock); //Aplicamos las cajas al bloque resultante anterior
	permutacionF_DES(&pBlock, &bloqueTrasSBOX); //Terminamos con la ultima permutacion de F_DES, 32bits

	//Recordamos que L(i) = R(i-1). Asique copiamos el bloque para preparar la salida
	copiarBloque(izqSalida, derEntrada, BITS_IN_FEISTEL / 2);

	//Y recordamos que R(i) = L(i-1) + F(R(i-1), k(i)), siendo el mas la suma exclusiva
	xorDES(derSalida, izqEntrada, &pBlock, BITS_IN_P);
}

void expansion(Bloque* resultado, Bloque* entrada) {
	permutacion(resultado, entrada, E, BITS_IN_E);
}

void copiarBloque(Bloque* resultado, Bloque* entrada, int lon) {

	int i;

	for (i = 1; i <= lon; i++) resultado->bloque[i] = entrada->bloque[i];
}

void unirBloques(Bloque* bloque, Bloque* medizq, Bloque* medder) {

	int i;

	for (i = 1; i <= BITS_IN_FEISTEL / 2; i++) {
		bloque->bloque[i] = medizq->bloque[i];
		bloque->bloque[BITS_IN_FEISTEL / 2 + i] = medder->bloque[i];
	}
}

void swap(Bloque* resultado, Bloque* entrada) {
	permutacion(resultado, entrada, SWAP, BITS_IN_SWAP);
}

void invPermArranque(Bloque* resultado, Bloque* entrada) {
	permutacion(resultado, entrada, IP_INV, BITS_IN_IP);
}



void SBox(Bloque* resultado, Bloque* entrada) {

	int i, fila, col;
	int value;

	for (i = 0; i < NUM_S_BOXES; i++) {
		fila = 2 * entrada->bloque[1 + 6 * i] + entrada->bloque[1 + 6 * i + 5];
		col = 8 * entrada->bloque[1 + 6 * i + 1] + 4 * entrada->bloque[1 + 6 * i + 2];
		col +=  2 * entrada->bloque[1 + 6 * i + 3] + entrada->bloque[1 + 6 * i + 4];
		
		value = S_BOXES[i][fila][col];
		resultado->bloque[1 + i * 4 + 3] = value % 2;
		resultado->bloque[1 + i * 4 + 2] = (value / 2) % 2;
		resultado->bloque[1 + i * 4 + 1] = (value / 4) % 2;
		resultado->bloque[1 + i * 4] = (value / 8) % 2;
	}
}

void permutacionF_DES(Bloque* resultado, Bloque* entrada) {
	permutacion(resultado, entrada, P, BITS_IN_P);
}




