#include "EfectoAvalanchaDES.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

/* PC1 "permutation" */
static const unsigned short PC1[BITS_IN_PC1] = { 57, 49, 41, 33, 25, 17, 9, 1,
		58, 50, 42, 34, 26, 18, 10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52,
		44, 36, 63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46, 38, 30, 22, 14, 6,
		61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4 };

/* PC2 "permutation" */
static const unsigned short PC2[BITS_IN_PC2] = { 14, 17, 11, 24, 1, 5, 3, 28,
		15, 6, 21, 10, 23, 19, 12, 4, 26, 8, 16, 7, 27, 20, 13, 2, 41, 52, 31,
		37, 47, 55, 30, 40, 51, 45, 33, 48, 44, 49, 39, 56, 34, 53, 46, 42, 50,
		36, 29, 32 };

/* Shifts in each of the key halves in each round (for encryption) */
static const unsigned short ROUND_SHIFTS[NUM_ROUNDS] = { 1, 1, 2, 2, 2, 2, 2, 2,
		1, 2, 2, 2, 2, 2, 2, 1 };

/* Shifts in each of the key halves in each round (for decryption) */
static const unsigned short ROUND_SHIFTS_DEC[NUM_ROUNDS] = { 0, 1, 2, 2, 2, 2,
		2, 2, 1, 2, 2, 2, 2, 2, 2, 1 };

/* IP permutation */
static const unsigned short IP[BITS_IN_IP] =
		{ 58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4, 62, 54,
				46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8, 57, 49,
				41, 33, 25, 17, 9, 1, 59, 51, 43, 35, 27, 19, 11, 3, 61, 53, 45,
				37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7 };

/* E expansion */
static const unsigned short E[BITS_IN_E] = { 32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8,
		9, 8, 9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17, 16, 17, 18, 19, 20, 21,
		20, 21, 22, 23, 24, 25, 24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1 };

/* P permutation */
static const unsigned short P[BITS_IN_P] = { 16, 7, 20, 21, 29, 12, 28, 17, 1,
		15, 23, 26, 5, 18, 31, 10, 2, 8, 24, 14, 32, 27, 3, 9, 19, 13, 30, 6,
		22, 11, 4, 25 };

/* S-boxes */
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

	int nPruebas, salida;
	char ficherosalida[MAX_NAME_LENGTH];
	FILE* fsalida = NULL;
	Diferencias dif;

	//Comprobamos que el numero de parametros de entrada es el adecuado
	if ((argc != 3) && (argc != 5)) {
		printf("El numero de parametros es erroneo.\n");
		printf(
				"Uso: ./EfectoAvalanchaDES {-n numero de pruebas} [-o fichero de salida] \n");
		return -1;
	}

	//Cogemos los argumentos de entrada
	if (getArgs(argc, argv, &nPruebas, ficherosalida, &salida) == -1) {
		printf("Los argumentos son erroneos.\n");
		printf(
				"Uso: ./EfectoAvalanchaDES {-n numero de pruebas} [-o fichero de salida] \n");
		return -1;
	}

	//Comprobamos que el numero de pruebas es positivo
	if (nPruebas < 1) {
		printf("El numero de pruebas debe ser positivo.\n");
		return -1;
	}

	if (salida) {
		if ((fsalida = fopen(ficherosalida, "w")) == NULL) {
			fprintf(stderr, "ERROR: Unable to open %s\n", ficherosalida);
			return -1;
		}
	} else
		fsalida = stdout;

	//Calculamos a ver el numer medio de bits que cambia en cada ronda
	//Al cambiar 1 bit
	calcularEstadisticas(&dif, nPruebas);
	imprimirSalida(fsalida, &dif);

	if (fsalida && (fsalida != stdout))
		fclose(fsalida);

	return 0;
}

int getArgs(int nArgs, char** args, int* nPruebas, char* ficherosalida,
		int* salida) {

	if (getEntero(nArgs, args, nPruebas, "-n", 2) != 1)
		return -1;

	if (nArgs == 3)
		*salida = 0;
	else if (((*salida) = getCadena(nArgs, args, ficherosalida, "-o", 2)) != 1)
		return -1;

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

	for (i = 1; i <= (nArgs - 2); i++) {

		if ((strncmp(args[i], modo, longitud) == 0)
				&& (strlen(args[i]) == longitud)) {
			if (flag)
				return -1;
			else {
				*entero = atoi(args[i + 1]);
				if ((*entero == 0) && !isdigit(args[i + 1][0]))
					return -1;
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
		if ((strncmp(args[i], modo, longitud) == 0)
				&& (strlen(args[i]) == longitud)) {
			if (flag)
				return -1;
			else {
				strcpy(cadena, args[i + 1]);
				flag = 1;
			}
		}

	return flag;
}

void calcularEstadisticas(Diferencias* dif, int nPruebas) {

	BloqueDES clave, input;
	int i;
	unsigned long int bitsCambiadosTexto[NUM_ROUNDS + 1];
	unsigned long int bitsCambiadosClave[NUM_ROUNDS + 1];
	short unsigned int*** Sboxes = NULL;

	srand(time(NULL));

	//Preparamos dos estructuras para ver los cambios que se hacen en cada ronda
	for (i = 0; i < NUM_ROUNDS + 1; i++) {
		bitsCambiadosTexto[i] = 0;
		bitsCambiadosClave[i] = 0;
	}

	//Hacemos pruebas a ver cuanto cambian la clave y el texto
	//y lo guardamos en las estructuras que hemos creado
	Sboxes = guardarMemSboxes();
	for (i = 0; i < nPruebas; i++) {
		crearClave(&clave);
		crearBloque(&input, TAM_BLOQUE);
		pruebaTextoCambiado(&input, &clave, bitsCambiadosTexto, Sboxes);
		pruebaClaveCambiada(&input, &clave, bitsCambiadosClave, Sboxes);
	}
	freeSboxes(Sboxes);

	//Calculamos la media para cada ronda
	for (i = 0; i < NUM_ROUNDS + 1; i++) {
		dif->bitsCambiadosTexto[i] = (bitsCambiadosTexto[i])
				* (1. / nPruebas);
		dif->bitsCambiadosClave[i] = (bitsCambiadosClave[i])
				* (1. / nPruebas);
	}
}

void imprimirSalida(FILE* fsalida, Diferencias* dif) {

	int i;

	fprintf(fsalida,
			"Numero de bits que cambian tras cambiar un bit del texto:\n");
	for (i = 0; i < NUM_ROUNDS + 1; i++)
		fprintf(fsalida, "- Ronda %d:\t%.2f\n", i,
				dif->bitsCambiadosTexto[i]);

	fprintf(fsalida,
			"Numero de bits que cambian tras cambiar un bit de la clave:\n");
	for (i = 0; i < NUM_ROUNDS + 1; i++)
		fprintf(fsalida, "- Round %d:\t%.2f\n", i,
				dif->bitsCambiadosClave[i]);
}

void pruebaTextoCambiado(BloqueDES* entrada, BloqueDES* clave,
		unsigned long int* bitsCambiado, short unsigned int*** Sboxes) {

	BloqueDES bloqueIzq_pre, bloqueDer_pre, bloqueIzq_post, bloqueDer_post;
	BloqueDES bloqueTrasIP;
	BloqueDES clave_pre, clave_post, clave_ronda;
	BloqueDES entrada2;
	BloqueDES bloqueIzq2_pre, bloqueDer2_pre, bloqueIzq2_post, bloqueDer2_post;
	BloqueDES bloqueTrasIP2;
	int i;

	//Vamos a preparar la entrada cambiada
	copiarBloque(&entrada2, entrada, TAM_BLOQUE);
	cambioBit(&entrada2, TAM_BLOQUE);
	bitsCambiado[0] += calculaDiferencias(entrada, &entrada2, TAM_BLOQUE);

	//Aplicamos la permutacion inicial a la entrada y la entrada cambiada
	permInicial(&bloqueTrasIP, entrada);
	permInicial(&bloqueTrasIP2, &entrada2);

	//Dividimos los dos bloques ne mitades
	getParteIzquierda(&bloqueIzq_pre, &bloqueTrasIP);
	getParteDerecha(&bloqueDer_pre, &bloqueTrasIP);
	getParteIzquierda(&bloqueIzq2_pre, &bloqueTrasIP2);
	getParteDerecha(&bloqueDer2_pre, &bloqueTrasIP2);

	//PC1
	aplicarPC1(&clave_pre, clave);

	//Aplicamos las rondas DES por duplicado
	for (i = 1; i <= NUM_ROUNDS; i++) {
		//Aplicamos LCS
		aplicarLCS(&clave_post, &clave_pre, i, 1);
		//Aplicamos PC2
		aplicarPC2(&clave_ronda, &clave_post);

		//Aplicamos DES por duplicado
		aplicarDES(&bloqueIzq_post, &bloqueDer_post, &bloqueIzq_pre, &bloqueDer_pre, &clave_ronda,
				i, Sboxes);
		aplicarDES(&bloqueIzq2_post, &bloqueDer2_post, &bloqueIzq2_pre, &bloqueDer2_pre,
				&clave_ronda, i, Sboxes);

		//Calculamos diffs
		bitsCambiado[i] += calculaDiferencias(&bloqueIzq_post, &bloqueIzq2_post,
		BITS_IN_FEISTEL / 2);
		bitsCambiado[i] += calculaDiferencias(&bloqueDer_post, &bloqueDer2_post,
		BITS_IN_FEISTEL / 2);

		//Prepramos los argumentos para la siguiente iteracion
		copiarBloque(&bloqueIzq_pre, &bloqueIzq_post, BITS_IN_FEISTEL / 2);
		copiarBloque(&bloqueDer_pre, &bloqueDer_post, BITS_IN_FEISTEL / 2);
		copiarBloque(&bloqueIzq2_pre, &bloqueIzq2_post, BITS_IN_FEISTEL / 2);
		copiarBloque(&bloqueDer2_pre, &bloqueDer2_post, BITS_IN_FEISTEL / 2);
		copiarBloque(&clave_pre, &clave_post, BITS_IN_PC1);
	}
}

void pruebaClaveCambiada(BloqueDES* entrada, BloqueDES* clave,
		unsigned long int* bitsCambiado, short unsigned int*** Sboxes) {

	BloqueDES bloqueIzq_pre, bloqueDer_pre, bloqueIzq_post, bloqueDer_post;
	BloqueDES bloqueTrasIP;
	BloqueDES clave_pre, clave_post, clave_ronda;
	BloqueDES clave2;
	BloqueDES clave2_pre, clave2_post, clave2_ronda;
	BloqueDES bloqueIzq2_pre, bloqueDer2_pre, bloqueIzq2_post, bloqueDer2_post;
	int i;

	//Vamos a preparar la clave cambiada
	copiarBloque(&clave2, clave, TAM_CLAVE);
	cambioBit(&clave2, TAM_CLAVE);	//Cambiamos 1 bit

	//Aplicamos la permutacion inicial a la entrada
	permInicial(&bloqueTrasIP, entrada);

	//Dividimos el bloque en dos mitades. Guardamos estas mitades
	//por duplicado para usarlos con cada clave
	getParteIzquierda(&bloqueIzq_pre, &bloqueTrasIP);
	getParteDerecha(&bloqueDer_pre, &bloqueTrasIP);
	copiarBloque(&bloqueIzq2_pre, &bloqueIzq_pre, BITS_IN_FEISTEL / 2);
	copiarBloque(&bloqueDer2_pre, &bloqueDer_pre, BITS_IN_FEISTEL / 2);

	//Aplicamos PC1
	aplicarPC1(&clave_pre, clave);
	aplicarPC1(&clave2_pre, &clave2);

	//Aplicamos las rondas DES por duplicado, una vez con la clave correcta
	//y otra con la clave con el bit cambiado
	for (i = 1; i <= NUM_ROUNDS; i++) {

		//Aplicamos LCS
		aplicarLCS(&clave_post, &clave_pre, i, 1);
		aplicarLCS(&clave2_post, &clave2_pre, i, 1);

		//Aplicamos PC2
		aplicarPC2(&clave_ronda, &clave_post);
		aplicarPC2(&clave2_ronda, &clave2_post);

		//Aplicamos F de DES
		aplicarDES(&bloqueIzq_post, &bloqueDer_post, &bloqueIzq_pre,
				&bloqueDer_pre, &clave_ronda, i, Sboxes);
		aplicarDES(&bloqueIzq2_post, &bloqueDer2_post, &bloqueIzq2_pre, &bloqueDer2_pre,
				&clave2_ronda, i, Sboxes);

		//Calculamos las diferencias en esta ronda en cada mitad del bloque
		bitsCambiado[i] += calculaDiferencias(&bloqueIzq_post, &bloqueIzq2_post,
		BITS_IN_FEISTEL / 2);
		bitsCambiado[i] += calculaDiferencias(&bloqueDer_post, &bloqueDer2_post,
		BITS_IN_FEISTEL / 2);

		//Prepramos los datos para otra iteracion
		copiarBloque(&bloqueIzq_pre, &bloqueIzq_post, BITS_IN_FEISTEL / 2);
		copiarBloque(&bloqueDer_pre, &bloqueDer_post, BITS_IN_FEISTEL / 2);
		copiarBloque(&bloqueIzq2_pre, &bloqueIzq2_post, BITS_IN_FEISTEL / 2);
		copiarBloque(&bloqueDer2_pre, &bloqueDer2_post, BITS_IN_FEISTEL / 2);
		copiarBloque(&clave_pre, &clave_post, BITS_IN_PC1);
		copiarBloque(&clave2_ronda, &clave2_post, BITS_IN_PC1);
	}
}

void crearClave(BloqueDES* clave) {

	int byte, bit;
	int acc;

	//La clave es de 64 bits. Recordamos que no usamos la posicion [0] de un bloque
	for (byte = 0; byte < (TAM_CLAVE / 8); byte++) {
		acc = 0;
		for (bit = 0; bit < 7; bit++)
			acc += (clave->bloque[8 * byte + bit + 1] = naleatorio(0, 1));
		clave->bloque[8 * byte + 7 + 1] = (acc % 2 == 0);
	}
}

void crearBloque(BloqueDES* b, int tamBloque) {

	int i;

	for (i = 1; i <= tamBloque; i++)
		b->bloque[i] = naleatorio(0, 1);
}

void copiarBloque(BloqueDES* resultado, BloqueDES* entrada, int length) {

	int i;

	for (i = 1; i <= length; i++)
		resultado->bloque[i] = entrada->bloque[i];
}

void cambioBit(BloqueDES* b, int tamBloque) {

	int i = naleatorio(1, tamBloque);

	if (b->bloque[i] == 0)
		b->bloque[i] = 1;
	else
		b->bloque[i] = 0;
}

int calculaDiferencias(BloqueDES* b1, BloqueDES* b2, int tamBloque) {

	int i;
	int n = 0;

	for (i = 1; i <= tamBloque; i++)
		if (b1->bloque[i] != b2->bloque[i])
			n++;

	return n;
}

void permInicial(BloqueDES* resultado, BloqueDES* entrada) {
	//Hacemos la permutacion con los datos de la permutacion Inicial
	aplicarMatriz(resultado, entrada, IP, BITS_IN_IP);
}

void getParteIzquierda(BloqueDES* izq, BloqueDES* entrada) {

	int i;

	for (i = 1; i <= BITS_IN_FEISTEL / 2; i++)
		izq->bloque[i] = entrada->bloque[i];
}

void getParteDerecha(BloqueDES* der, BloqueDES* entrada) {

	int i;

	for (i = 1; i <= BITS_IN_FEISTEL / 2; i++)
		der->bloque[i] = entrada->bloque[i + BITS_IN_FEISTEL / 2];
}

void aplicarPC1(BloqueDES* new, BloqueDES* old) {
	aplicarMatriz(new, old, PC1, BITS_IN_PC1);
}

void aplicarLCS(BloqueDES* resultado, BloqueDES* entrada, int nRonda, int modo) {

	if (modo == 1)
		desplazamientoIzq(resultado, entrada, ROUND_SHIFTS[nRonda - 1]);
	else if (modo == 2)
		desplazamientoDer(resultado, entrada, ROUND_SHIFTS_DEC[nRonda - 1]);
}

void aplicarPC2(BloqueDES* new, BloqueDES* old) {
	aplicarMatriz(new, old, PC2, BITS_IN_PC2);
}

void aplicarDES(BloqueDES* resIzq, BloqueDES* resDer,
		BloqueDES* entradaIzq, BloqueDES* entradaDer, BloqueDES* key, int nRonda,
		short unsigned int*** Sbox) {

	BloqueDES bloque1, bloque2, bloque3, bloque4;

	//F de DES sobre el semibloque derecho
	expansion(&bloque1, entradaDer);
	xorDES(&bloque2, &bloque1, key, BITS_IN_E);
	SBoxGeneral(&bloque3, &bloque2, Sbox);
	aplicarP(&bloque4, &bloque3);

	// L(i) = R(i - 1)
	copiarBloque(resIzq, entradaDer, BITS_IN_FEISTEL / 2);

	// R(i) = L(i - 1) XOR F(R(i - 1), k(i))
	xorDES(resDer, entradaIzq, &bloque4, BITS_IN_P);
}

void aplicarMatriz(BloqueDES* resultado, BloqueDES* entrada,
		const unsigned short* indices, int length) {

	int i;

	for (i = 1; i <= length; i++)
		resultado->bloque[i] = entrada->bloque[indices[i - 1]];
}

void desplazamientoIzq(BloqueDES* resultado, BloqueDES* entrada, int shift) {

	int i;

	for (i = 0; i < BITS_IN_KEY/2; i++)
		resultado->bloque[i + 1] = entrada->bloque[((i + shift)
				% (BITS_IN_KEY/2)) + 1];
	for (i = 0; i < BITS_IN_KEY/2; i++)
		resultado->bloque[i + BITS_IN_KEY/2 + 1] = entrada->bloque[((i
				+ shift) % (BITS_IN_KEY/2)) + BITS_IN_KEY/2 + 1];
}

void desplazamientoDer(BloqueDES* resultado, BloqueDES* entrada, int shift) {

	int i;

	for (i = 0; i < BITS_IN_KEY/2; i++)
		resultado->bloque[i + 1] = entrada->bloque[((i - shift
				+ BITS_IN_KEY/2) % (BITS_IN_KEY/2)) + 1];
	for (i = 0; i < BITS_IN_KEY/2; i++)
		resultado->bloque[i + BITS_IN_KEY/2 + 1] = entrada->bloque[((i
				- shift + BITS_IN_KEY/2) % (BITS_IN_KEY/2))
				+ BITS_IN_KEY/2 + 1];
}

void expansion(BloqueDES* resultado, BloqueDES* entrada) {
	aplicarMatriz(resultado, entrada, E, BITS_IN_E);
}

void xorDES(BloqueDES* resultado, BloqueDES* a, BloqueDES* b, int length) {

	int i;

	for (i = 1; i <= length; i++)
		resultado->bloque[i] = (a->bloque[i] != b->bloque[i]);
}

void SBoxGeneral(BloqueDES* resultado, BloqueDES* entrada,
		short unsigned int*** Sbox) {

	int i, fila, col;
	int valor;

	for (i = 0; i < NUM_S_BOXES; i++) {
		fila = 2 * entrada->bloque[1 + 6 * i] + entrada->bloque[1 + 6 * i + 5];
		col = 8 * entrada->bloque[1 + 6 * i + 1]
				+ 4 * entrada->bloque[1 + 6 * i + 2]
				+ 2 * entrada->bloque[1 + 6 * i + 3]
				+ entrada->bloque[1 + 6 * i + 4];

		valor = Sbox[i][fila][col];
		resultado->bloque[1 + i * 4 + 3] = valor % 2;
		resultado->bloque[1 + i * 4 + 2] = (valor / 2) % 2;
		resultado->bloque[1 + i * 4 + 1] = (valor / 4) % 2;
		resultado->bloque[1 + i * 4] = (valor / 8) % 2;
	}
}

void aplicarP(BloqueDES* resultado, BloqueDES* entrada) {
	aplicarMatriz(resultado, entrada, P, BITS_IN_P);
}

int naleatorio(int a, int b) {

	if (a >= b)
		return a;

	return a + (rand() % (b - a + 1));
}

unsigned short*** guardarMemSboxes() {

	unsigned short*** Sboxes;
	int i, j;
	//Reservamos memoria para guardas las cajas S
	//Recordamos que cada caja es una matriz por lo que necesitaremos
	//un array del tipo SBOXES[i][j][k], siendo SBOX = SBOXES[i]
	Sboxes = (unsigned short***) malloc(NUM_S_BOXES * sizeof(unsigned short**));
	for (i = 0; i < NUM_S_BOXES; i++) {
		Sboxes[i] = (unsigned short**) malloc(
		ROWS_PER_SBOX * sizeof(unsigned short*));	//Filas
		for (j = 0; j < ROWS_PER_SBOX; j++)
			Sboxes[i][j] = (unsigned short*) malloc(
			COLUMNS_PER_SBOX * sizeof(unsigned short));	//Columnas
	}

	getSboxes(Sboxes);

	return Sboxes;
}

void freeSboxes(unsigned short*** Sboxes) {

	int i, j;

	for (i = 0; i < NUM_S_BOXES; i++) {
		for (j = 0; j < ROWS_PER_SBOX; j++)
			free(Sboxes[i][j]);
		free(Sboxes[i]);
	}

	free(Sboxes);
	Sboxes = NULL;
}

void getSboxes(unsigned short*** Sboxes) {

	int box, r, c;

	for (box = 0; box < NUM_S_BOXES; box++)
		for (r = 0; r < ROWS_PER_SBOX; r++)
			for (c = 0; c < COLUMNS_PER_SBOX; c++)
				Sboxes[box][r][c] = S_BOXES[box][r][c];
}

