#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "desECB.h"

/* "permutacion" PC1 */
static const unsigned short PC1[BITS_IN_PC1] = { 57, 49, 41, 33, 25, 17, 9, 1,
		58, 50, 42, 34, 26, 18, 10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52,
		44, 36, 63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46, 38, 30, 22, 14, 6,
		61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4 };

/* "permutacion" PC2 */
static const unsigned short PC2[BITS_IN_PC2] = { 14, 17, 11, 24, 1, 5, 3, 28,
		15, 6, 21, 10, 23, 19, 12, 4, 26, 8, 16, 7, 27, 20, 13, 2, 41, 52, 31,
		37, 47, 55, 30, 40, 51, 45, 33, 48, 44, 49, 39, 56, 34, 53, 46, 42, 50,
		36, 29, 32 };

/* numero de bits que hay que rotar cada semiclave segun el numero de ronda */
static const unsigned short ROUND_SHIFTS[ROUNDS] = { 1, 1, 2, 2, 2, 2, 2, 2, 1,
		2, 2, 2, 2, 2, 2, 1 };

/* permutacion IP */
static const unsigned short IP[BITS_IN_IP] =
		{ 58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4, 62, 54,
				46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8, 57, 49,
				41, 33, 25, 17, 9, 1, 59, 51, 43, 35, 27, 19, 11, 3, 61, 53, 45,
				37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7 };

/* inversa de IP */
static const unsigned short IP_INV[BITS_IN_IP] = { 40, 8, 48, 16, 56, 24, 64,
		32, 39, 7, 47, 15, 55, 23, 63, 31, 38, 6, 46, 14, 54, 22, 62, 30, 37, 5,
		45, 13, 53, 21, 61, 29, 36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11,
		51, 19, 59, 27, 34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41, 9, 49, 17, 57,
		25 };

/* expansion E */
static const unsigned short E[BITS_IN_E] = { 32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8,
		9, 8, 9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17, 16, 17, 18, 19, 20, 21,
		20, 21, 22, 23, 24, 25, 24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1 };

/* permutacion P */
static const unsigned short P[BITS_IN_P] = { 16, 7, 20, 21, 29, 12, 28, 17, 1,
		15, 23, 26, 5, 18, 31, 10, 2, 8, 24, 14, 32, 27, 3, 9, 19, 13, 30, 6,
		22, 11, 4, 25 };

/* cajas S */
static const unsigned short S_BOXES[NUM_S_BOXES][ROWS_PER_SBOX][COLUMNS_PER_SBOX] =
		{ { { 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7 }, { 0, 15,
				7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8 }, { 4, 1, 14, 8,
				13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0 }, { 15, 12, 8, 2, 4, 9,
				1, 7, 5, 11, 3, 14, 10, 0, 6, 13 } }, { { 15, 1, 8, 14, 6, 11,
				3, 4, 9, 7, 2, 13, 12, 0, 5, 10 }, { 3, 13, 4, 7, 15, 2, 8, 14,
				12, 0, 1, 10, 6, 9, 11, 5 }, { 0, 14, 7, 11, 10, 4, 13, 1, 5, 8,
				12, 6, 9, 3, 2, 15 }, { 13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12,
				0, 5, 14, 9 } },

		{ { 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8 }, { 13, 7, 0,
				9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1 }, { 13, 6, 4, 9, 8,
				15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7 }, { 1, 10, 13, 0, 6, 9, 8,
				7, 4, 15, 14, 3, 11, 5, 2, 12 } },

		{ { 7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15 }, { 13, 8, 11,
				5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9 }, { 10, 6, 9, 0, 12,
				11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4 }, { 3, 15, 0, 6, 10, 1, 13,
				8, 9, 4, 5, 11, 12, 7, 2, 14 } }, { { 2, 12, 4, 1, 7, 10, 11, 6,
				8, 5, 3, 15, 13, 0, 14, 9 }, { 14, 11, 2, 12, 4, 7, 13, 1, 5, 0,
				15, 10, 3, 9, 8, 6 }, { 4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5,
				6, 3, 0, 14 }, { 11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4,
				5, 3 } }, { { 12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5,
				11 }, { 10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8 },
				{ 9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6 }, { 4,
						3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13 } },
				{ { 4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1 }, {
						13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6 },
						{ 1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2 },
						{ 6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12 } },
				{ { 13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7 }, { 1,
						15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2 }, {
						7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8 },
						{ 2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11 } } };

int main(int argc, char** argv) {

	FILE *fentrada = NULL, *fsalida = NULL;
	unsigned long lon, n_bloques, aux = 0;
	unsigned short int padding;
	uint8_t * clave = NULL;
	int modo, i, j, entrada = 0, salida;
	char ficheroentrada[MAX_NOMBRE], ficherosalida[MAX_NOMBRE];
	char texto_plano[MAX_TEXTO];

	//Comprobamos que al menos el numero de argumentos es el correcto
	if ((argc != 2) && (argc != 4) && (argc != 6) && (argc != 8)) {
		printf("Numero de argumentos incorrecto.\n");
		printf(
				"Uso: ./desECB {-C | -D -k clave} [-i fichero entrada] [-o fichero salida]\n");
		return -1;
	}

	if (getArgs(argc, argv, &modo, ficheroentrada, &entrada, ficherosalida,
			&salida) == -1) {
		printf("Numero de argumentos incorrecto.\n");
		printf(
				"Uso: ./desECB {-C | -D -k clave} [-i fichero entrada] [-o fichero salida]\n");
		return -1;
	}

	//Me estaba haciendo un lio con los punteros para la clave, la reserva de memoria
	//no se porque, asique saque el bucle fuera de getArgs
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-C") == 0) {

			clave = (uint8_t*) malloc(TAM_CLAVE * sizeof(char));
			generaClave(clave);

			printf("\nClave Hexadecimal: \n0x");
			for (j = 0; j < TAM_CLAVE; j++) {
				printf("%02X", clave[j]);
			}
			printf("\n");
		} else if (strcmp(argv[i], "-D") == 0) {

			i += 1;
			if (strcmp(argv[i], "-k") == 0) {
				i += 1;
				if (strlen(argv[i]) != TAM_CLAVE * 2) {
					printf("\nClave no valida.");
					return -1;
				} else {
					clave = (uint8_t*) malloc(
					TAM_CLAVE * sizeof(char));
					unsigned int u;
					for (j = 0; j < TAM_CLAVE * 2; j += 2) {
						sscanf(argv[i] + j, "%02X", &u);
						clave[j / 2] = u;
					}
				}
			}
		}
	}
	//Abrimos el fichero de entrada
	if (entrada) {
		if ((fentrada = fopen(ficheroentrada, "rb")) == NULL) {
			fprintf(stderr, "Error al abrir el fichero: %s .\n",
					ficheroentrada);
			return -1;
		}
	} else
		fentrada = stdin;
	//Abrimos el fichero de salida
	if (salida) {
		if ((fsalida = fopen(ficherosalida, "wb")) == NULL) {
			fprintf(stderr, "Error al abrir el fichero: %s .\n", ficherosalida);
			return -1;
		}
	} else
		fsalida = stdout;
	printf("Entrada: %s Salida: %s\n", ficheroentrada, ficherosalida);

	//Preparamos la estructura auxiliar declarada para guardar la descomposicion de la clave a lo largo del aplicarDES
	DescomposicionClave* subclaves = (DescomposicionClave*) malloc(
			17 * sizeof(DescomposicionClave));
	tratarClave(clave, subclaves);

	//Preparamos memoria para los bloques con los que trabajaremos
	uint8_t* bloque_entrada = (uint8_t*) malloc(8 * sizeof(int));
	uint8_t* bloque_salida = (uint8_t*) malloc(8 * sizeof(int));

	if (!entrada) {

		printf(
				"\nIntroduzca el texto con el que quiere trabaja r:(max long %d):\n",
				MAX_TEXTO);
		fentrada = stdin;

		while (fgets(texto_plano, 1024, fentrada) != NULL) {
			fentrada = fopen("stdin", "wb");	
			fwrite(texto_plano, 1, strlen(texto_plano), fentrada);//Metemos el texto en un fichero 'stdin'
		}

		fclose(fentrada);	
		fentrada = fopen("stdin", "rb");	
	}

	//Calculamos el tamanyo del archivo con fseek que nos permite ir al final del archivo
	fseek(fentrada, 0L, SEEK_END);
	//ftell nos dira el tamanyo
	lon = ftell(fentrada);
	//Volvemos el puntero al inicio para poder trabajar con el fichero
	fseek(fentrada, 0L, SEEK_SET);

	n_bloques = lon / TAM_CLAVE + ((lon % TAM_CLAVE) ? 1 : 0);

	//Hacemos un bucle que vaya cogiendo los bloques a los que aplicar aplicarDES
	while (fread(bloque_entrada, 1, 8, fentrada)) {
		aux++;//Llevamos la cuenta de cuantos bloques llevamos para tener en cuenta el padding
		if (aux == n_bloques) {	//En la ultima iteracion realizamos el padding como sea necesario
			//Ciframos
			if (modo == 1) {
				padding = 8 - lon % 8;//Comprobamos cuantos bits nos falta para completar bloque
				if (padding != 8) {
					//Hacemos el padding
					memset((bloque_entrada + 8 - padding), (uint8_t) padding,
							padding);
				}
				//Aplicamos DES
				aplicarDES(bloque_entrada, bloque_salida, subclaves, modo);
				
				fwrite(bloque_salida, 1, 8, fsalida);

				//Si tiene un tam divisible entre 8 meto un bloque adicional para padding
				if (padding == 8) {
					memset(bloque_entrada, (uint8_t) padding, 8);
					aplicarDES(bloque_entrada, bloque_salida, subclaves, modo);
					fwrite(bloque_salida, 1, 8, fsalida);
				}
			} else if (modo == 2) {			//Desciframos
				aplicarDES(bloque_entrada, bloque_salida, subclaves, modo);
				padding = bloque_salida[7];

				if (padding < 8) {
					fwrite(bloque_salida, 1, 8 - padding, fsalida);
				}
			}
		} else {			
			aplicarDES(bloque_entrada, bloque_salida, subclaves, modo);
			fwrite(bloque_salida, 1, 8, fsalida);
		}
		memset(bloque_salida, 0, 8);			
	}
	//Liberamos  memoria
	free(clave);
	free(subclaves);
	free(bloque_entrada);
	free(bloque_salida);

	//Cerramos los ficheros
	if (entrada)
		fclose(fentrada);
	if (salida)
		fclose(fsalida);

	return 0;

}
/*--------------------------------------------------------------------------
 Obtiene los argumentos
 - Entrada:
	 * Número de argumentos
	 * Array de argumentos
	 * Puntero al modo
	 * Puntero al fichero de salida
	 * Puntero al fichero de entrada
	 * Fichero de salida
	 * Fichero de entrada
 - Salida:
	 * -1 si ocurre algun error
	 * 1 si va bien
 --------------------------------------------------------------------------*/
int getArgs(int nArgs, char** args, int* modo, char* ficheroentrada,
		int* entrada, char* ficherosalida, int* salida) {

	if (getModo(nArgs, args, modo) != 1)
		return -1;

	if (((nArgs == 2) && (*modo == 1)) || ((nArgs == 4) && (*modo == 2))) {
		*entrada = 0;
		*salida = 0;
	} else {
		if (((*entrada) = getCadena(nArgs, args, ficheroentrada, "-i", 2))
				== -1) {
			return -1;
		}
		if (((*salida) = getCadena(nArgs, args, ficherosalida, "-o", 2))
				== -1) {
			return -1;
		}
		if ((((nArgs == 4) && (*modo == 1)) || ((nArgs == 6) && (*modo == 2)))
				&& (*entrada + *salida != 1)) {
			printf("Entrada.\n");
			return -1;
		}
		if ((((nArgs == 6) && (*modo == 1)) || ((nArgs == 8) && (*modo == 2)))
				&& (*entrada + *salida != 2)) {
			printf("Salida.\n");
			return -1;
		}
	}

	return 0;
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

	for (i = 1; i < nArgs; i++) {
		if ((strncmp(args[i], "-C", 2) == 0) && (strlen(args[i]) == 2)) {
			if (flag)
				return -1;
			else {
				*modo = 1;
				flag = 1;
			}
		} else if (strncmp(args[i], "-D", 2) == 0) {
			if (flag)
				return -1;
			else {
				*modo = 2;
				flag = 1;
			}
		}
	}
	return flag;
}
/*--------------------------------------------------------------------------
 Obtiene una cadena
 - Entrada:
	 * Número de argumentos
	 * Array de argumentos
	 * Cadena a obtener
	 * Longitud
	 * Flag
 - Salida:
	 * -1 si ocurre algun error
	 * 1 si va bien
 --------------------------------------------------------------------------*/
int getCadena(int nArgs, char** args, char* cadena, char* flag, int longitud) {

	int i;
	int aux = 0;

	for (i = 1; i <= (nArgs - 2); i++)
		if ((strncmp(args[i], flag, longitud) == 0)
				&& (strlen(args[i]) == longitud)) {
			if (aux)
				return -1;
			else {
				strcpy(cadena, args[i + 1]);
				aux = 1;
			}
		}
	return aux;
}
/*--------------------------------------------------------------------------
 Genera una clave
 - Entrada:
	 * Clave que va a ser generada
 --------------------------------------------------------------------------*/
void generaClave(uint8_t* clave) {
	int i;
	unsigned int aux;

	//Preparamos la inicializacion de rand
	aux = (unsigned int) time(NULL);
	srand(aux);
	//Creamos una clave con numeros hexadecimales aleatorios
	for (i = 0; i < TAM_CLAVE; i++) {
		clave[i] = rand() % 255;
	}
}
/*--------------------------------------------------------------------------
 Se realiza el tratamiento de la clave: aplicando PC1, diviendo en subclaves y 
 haciendo el LCS a cada subclaves
 - Entrada:
	 * Clave
	 * Subclaves en que se dividira la clave
 --------------------------------------------------------------------------*/
void tratarClave(uint8_t* clave, DescomposicionClave* subclaves) {

	cleanDescomposicionClave(subclaves);
	//Se comprime y permuta la clave (pasa de 64 bits a 56)
	aplicarPC1(clave, subclaves);
	//Se divide en las subclaves C y D
	dividirClave(subclaves);
	//Se realiza el LCS de las subclaves
	aplicarLCS(subclaves);

	return;
}
/*--------------------------------------------------------------------------
 Se borra la descomposición de las subclaves
 - Entrada:
	 * Subclaves
 --------------------------------------------------------------------------*/
void cleanDescomposicionClave(DescomposicionClave* subclaves) {
	int i;

	for (i = 0; i < 8; i++) {
		subclaves[0].k[i] = 0;
	}
}
/*--------------------------------------------------------------------------
 Permuta y comprime la clave
 - Entrada:
	 * Clave
	 * Subclaves
 --------------------------------------------------------------------------*/
void aplicarPC1(uint8_t* clave, DescomposicionClave* subclaves) {
	int i, desp;
	uint8_t byte_desp;

	for (i = 0; i < BITS_IN_PC1; i++) {
		desp = PC1[i];
		//Mascara para tomar el bit de la posicion correcta de la clave
		byte_desp = 0x80 >> ((desp - 1) % 8);
		//Cogemos el bit de nuestra clave
		byte_desp &= clave[(desp - 1) / 8];
		//Si era un 1 va al principio si era 0 se queda asi
		byte_desp <<= ((desp - 1) % 8);
		//Metemos en nuestra subclave el bit obtenido en la posicion que le corresponde (mod 8)
		subclaves[0].k[i / 8] |= (byte_desp >> i % 8);
	}

}
/*--------------------------------------------------------------------------
 Divide la clave en las subclaves C y D
 - Entrada:
	 * Subclaves
 --------------------------------------------------------------------------*/
void dividirClave(DescomposicionClave* subclaves) {
	int i;
	
	//C corresponde a los 28 primeros bits de K
	for (i = 0; i < 3; i++) {
		subclaves[0].c[i] = subclaves[0].k[i];
	}
	//Por tanto llega hasta la primera mitad del cuarto byte de K
	subclaves[0].c[3] = subclaves[0].k[3] & 0xF0;

	//D usa los 28 bits siguientes
	for (i = 0; i < 3; i++) {
		subclaves[0].d[i] = (subclaves[0].k[i + 3] & 0x0F) << 4;
		subclaves[0].d[i] |= (subclaves[0].k[i + 4] & 0xF0) >> 4;
	}
	subclaves[0].d[3] = (subclaves[0].k[6] & 0x0F) << 4;
}
/*--------------------------------------------------------------------------
 Aplica el LCS a las subclaves C y D
 - Entrada:
	 * Subclaves
 --------------------------------------------------------------------------*/
void aplicarLCS(DescomposicionClave* subclaves) {
	int i, j, desp;
	uint8_t byte_desp, bits_desp1, bits_desp2, bits_desp3, bits_desp4;

	//En cada una de las rondas
	for (i = 1; i < ROUNDS + 1; i++) {
		//Rellenamos con la subclave anterior
		for (j = 0; j < 4; j++) {
			subclaves[i].c[j] = subclaves[i - 1].c[j];
			subclaves[i].d[j] = subclaves[i - 1].d[j];
		}
		//Obtenemos el desplazamiento de la ronda
		desp = ROUND_SHIFTS[i];
		if (desp == 1) {
			byte_desp = 0x80;
		} else {
			byte_desp = 0xC0;
		}

		// Aqui tomo los bits que voy a desplazar en C
		bits_desp1 = byte_desp & subclaves[i].c[0];
		bits_desp2 = byte_desp & subclaves[i].c[1];
		bits_desp3 = byte_desp & subclaves[i].c[2];
		bits_desp4 = byte_desp & subclaves[i].c[3];

		//Desplazo el primero lo necesario a la izquierda y en sus huecos meto los desplazados del anterior
		subclaves[i].c[0] <<= desp;
		subclaves[i].c[0] |= (bits_desp2 >> (8 - desp));
		//Para el segundo
		subclaves[i].c[1] <<= desp;
		subclaves[i].c[1] |= (bits_desp3 >> (8 - desp));
		//Para el tercero
		subclaves[i].c[2] <<= desp;
		subclaves[i].c[2] |= (bits_desp4 >> (8 - desp));
		//Para el cuarto hay que tener en cuenta que es circular y que ademas solo valen los 4 primeros bits 
		subclaves[i].c[3] <<= desp;
		subclaves[i].c[3] |= (bits_desp1 >> (4 - desp));

		// Igual para D
		bits_desp1 = byte_desp & subclaves[i].d[0];
		bits_desp2 = byte_desp & subclaves[i].d[1];
		bits_desp3 = byte_desp & subclaves[i].d[2];
		bits_desp4 = byte_desp & subclaves[i].d[3];

		subclaves[i].d[0] <<= desp;
		subclaves[i].d[0] |= (bits_desp2 >> (8 - desp));

		subclaves[i].d[1] <<= desp;
		subclaves[i].d[1] |= (bits_desp3 >> (8 - desp));

		subclaves[i].d[2] <<= desp;
		subclaves[i].d[2] |= (bits_desp4 >> (8 - desp));

		subclaves[i].d[3] <<= desp;
		subclaves[i].d[3] |= (bits_desp1 >> (4 - desp));

		for (j = 0; j < BITS_IN_PC2; j++) {
			//Lo mismo para PC2 con 48 bits para las subclaves
			desp = PC2[j];
			 //Si es menor o igual que 28 es que estoy en C
			if (desp <= BITS_IN_PC1 / 2) {
				byte_desp = 0x80 >> ((desp - 1) % 8);
				byte_desp &= subclaves[i].c[(desp - 1) / 8];
				byte_desp <<= ((desp - 1) % 8);
			} else { //Si no estoy en D
				byte_desp = 0x80 >> ((desp - 29) % 8);
				byte_desp &= subclaves[i].d[(desp - 29) / 8];
				byte_desp <<= ((desp - 29) % 8);
			}
			subclaves[i].k[j / 8] |= (byte_desp >> j % 8);
		}

	}
}
/*--------------------------------------------------------------------------
 Aplicamos DES
 - Entrada:
	 * Bloque entrada
	 * Bloque salida
	 * Subclaves
	 * Modo
 --------------------------------------------------------------------------*/
void aplicarDES(uint8_t* bloque_entrada, uint8_t* bloque_salida,
		DescomposicionClave* subclaves, int modo) {
	
	uint8_t aux_ip[8], l[4], r[4];
	memset(bloque_salida, 0, 8);
	memset(aux_ip, 0, 8);

	//Realizamos la permutacion inicial
	aplicarIP(bloque_entrada, aux_ip);

	//Realizamos las rondas del DES
	aplicarRondaDES(aux_ip, modo, subclaves, r, l);
	
	//Realizamos la inversa de la permutacion inicial
	aplicarInversaIP(bloque_salida, aux_ip, r, l);

	return;

}
/*--------------------------------------------------------------------------
 Realizamos la permutacion inicial
 - Entrada:
	 * Bloque entrada
	 * Aux_IP
 --------------------------------------------------------------------------*/
void aplicarIP(uint8_t* bloque_entrada, uint8_t* aux_ip) {

	int desp, i;
	uint8_t byte_desp;

	//Seguimos un  procedimiento similar a con las claves
	for (i = 0; i < BITS_IN_IP; i++) {
		desp = IP[i];
		byte_desp = 0x80 >> ((desp - 1) % 8);
		byte_desp &= bloque_entrada[(desp - 1) / 8];
		byte_desp <<= ((desp - 1) % 8);

		aux_ip[i / 8] |= (byte_desp >> i % 8);
	}

	return;

}
/*--------------------------------------------------------------------------
 Realizamos la inversa de la permutacion inicial
 - Entrada:
	 * Bloque entrada
	 * Aux_IP
	 * L (de las rondas)
	 * R
 --------------------------------------------------------------------------*/
void aplicarInversaIP(uint8_t* bloque_salida, uint8_t* aux_ip, uint8_t* r,
		uint8_t* l) {

	int desp, i;
	uint8_t byte_desp;

	//Concatenamos invirtiendo el orden antes de aplicar IP_INV
	for (i = 0; i < 4; i++) {
		aux_ip[i] = r[i];
		aux_ip[4 + i] = l[i];
	}

	for (i = 0; i < BITS_IN_IP; i++) {
		desp = IP_INV[i];
		byte_desp = 0x80 >> ((desp - 1) % 8);
		byte_desp &= aux_ip[(desp - 1) / 8];
		byte_desp <<= ((desp - 1) % 8);
		bloque_salida[i / 8] |= (byte_desp >> i % 8);
	}

	return;

}
/*--------------------------------------------------------------------------
 Realizamos la ronda del DES
 - Entrada:
	 * Bloque entrada
	 * Aux_IP
 --------------------------------------------------------------------------*/
void aplicarRondaDES(uint8_t* aux_ip, int modo, DescomposicionClave* subclaves,
		uint8_t* r, uint8_t* l) {

	int i, k, desp, orden_clave;
	uint8_t li[4], ri[4], er[6], sbox_er[4], fila, col, byte_desp;

	//Una vez permutada la clave se divide en parte izquierda y parte derecha
	for (i = 0; i < 4; i++) {
		l[i] = aux_ip[i];
		r[i] = aux_ip[i + 4];
	}

	for (k = 1; k <= ROUNDS; k++) {
		//Li=Ri-1
		memcpy(li, r, 4);
		memset(er, 0, 6);
		//Con E procedemos como con IP
		for (i = 0; i < BITS_IN_E; i++) {
			desp = E[i];
			byte_desp = 0x80 >> ((desp - 1) % 8);
			byte_desp &= r[(desp - 1) / 8];
			byte_desp <<= ((desp - 1) % 8);
			er[i / 8] |= (byte_desp >> i % 8);
		}

		if (modo == 2) {
			orden_clave = 17 - k;
		} else {
			orden_clave = k;
		}

		//F=E(Ri-1) XOR Ki
		for (i = 0; i < 6; i++) {
			er[i] ^= subclaves[orden_clave].k[i];
		}

		memset(sbox_er, 0, 4);

		//Se van rellenando los 24 bits de la salida de las sboxes teniendo en cuenta por el que nos ibamos para aplicar correctamente las mascaras	

		// Byte 1
		fila = 0;

		fila |= ((er[0] & 0x80) >> 6);
		fila |= ((er[0] & 0x04) >> 2);

		col = 0;
		
		col |= ((er[0] & 0x78) >> 3);

		sbox_er[0] |= ((uint8_t) S_BOXES[0][fila][col] << 4);

		fila = 0;
		fila |= (er[0] & 0x02);
		fila |= ((er[1] & 0x10) >> 4);

		col = 0;
		col |= ((er[0] & 0x01) << 3);
		col |= ((er[1] & 0xE0) >> 5);

		sbox_er[0] |= (uint8_t) S_BOXES[1][fila][col];

		// Byte 2
		fila = 0;
		fila |= ((er[1] & 0x08) >> 2);
		fila |= ((er[2] & 0x40) >> 6);

		col = 0;
		col |= ((er[1] & 0x07) << 1);
		col |= ((er[2] & 0x80) >> 7);

		sbox_er[1] |= ((uint8_t) S_BOXES[2][fila][col] << 4);

		fila = 0;
		fila |= ((er[2] & 0x20) >> 4);
		fila |= (er[2] & 0x01);

		col = 0;
		col |= ((er[2] & 0x1E) >> 1);

		sbox_er[1] |= (uint8_t) S_BOXES[3][fila][col];

		// Byte 3
		fila = 0;
		fila |= ((er[3] & 0x80) >> 6);
		fila |= ((er[3] & 0x04) >> 2);

		col = 0;
		col |= ((er[3] & 0x78) >> 3);

		sbox_er[2] |= ((uint8_t) S_BOXES[4][fila][col] << 4);

		fila = 0;
		fila |= (er[3] & 0x02);
		fila |= ((er[4] & 0x10) >> 4);

		col = 0;
		col |= ((er[3] & 0x01) << 3);
		col |= ((er[4] & 0xE0) >> 5);

		sbox_er[2] |= (uint8_t) S_BOXES[5][fila][col];

		// Byte 4
		fila = 0;
		fila |= ((er[4] & 0x08) >> 2);
		fila |= ((er[5] & 0x40) >> 6);

		col = 0;
		col |= ((er[4] & 0x07) << 1);
		col |= ((er[5] & 0x80) >> 7);

		sbox_er[3] |= ((uint8_t) S_BOXES[6][fila][col] << 4);

		fila = 0;
		fila |= ((er[5] & 0x20) >> 4);
		fila |= (er[5] & 0x01);

		col = 0;
		col |= ((er[5] & 0x1E) >> 1);

		sbox_er[3] |= (uint8_t) S_BOXES[7][fila][col];

		memset(ri, 0, 4);

		for (i = 0; i < BITS_IN_P; i++) {
			desp = P[i];
			byte_desp = 0x80 >> ((desp - 1) % 8);
			byte_desp &= sbox_er[(desp - 1) / 8];
			byte_desp <<= ((desp - 1) % 8);

			ri[i / 8] |= (byte_desp >> i % 8);
		}

		for (i = 0; i < 4; i++) {
			ri[i] ^= l[i];
		}

		for (i = 0; i < 4; i++) {
			l[i] = li[i];
			r[i] = ri[i];
		}
	}
}

