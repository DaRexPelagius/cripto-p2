#include "3c.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

static const unsigned short OUTPUT_PAIRS[SBOX_OUTPUT_PAIRS][2] = { { 0, 1 }, {
		0, 2 }, { 0, 3 }, { 1, 2 }, { 1, 3 }, { 2, 3 } };

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
						{ 2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11 } }

		};

int main(int argc, char** argv) {

	int criterio, nPruebas, salida;
	char ficherosalida[MAX_NAME_LENGTH];
	FILE* fsalida = NULL;
	SAC SACdatos;
	BIC BICdatos;

	//Comprobamos que el numero de parametros de entrada es el adecuado
	if ((argc != 4) && (argc != 6)) {
		printf("Numero de argumentos incorrecto.\n");
		printf(
				"Uso: ./3c {-S|-B |-A} {-n numero de pruebas} [-o fichero salida]\n");
		return -1;
	}

	//Cogemos los argumentos de entrada
	if (getArgs(argc, argv, &criterio, &nPruebas, ficherosalida, &salida)
			== -1) {
		printf("Argumentos incorrectos.\n");
		printf(
				"Uso: ./3c {-S|-B|-A}  {-n numero de pruebas} [-o fichero salida]\n");
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

	//Segun el criterio que queramos estudiar hacemos unos datos u otros
	if (criterio == 1) {
		calcularEstadisticasSAC(&SACdatos, nPruebas);
		imprimirSalida(fsalida, NULL, &SACdatos, 1);
	} else if (criterio == 2) {
		calcularEstadisticasBIC(&BICdatos, nPruebas);
		imprimirSalida(fsalida, &BICdatos, NULL, 2);
	} else {
		calcularEstadisticasSAC(&SACdatos, nPruebas);
		calcularEstadisticasBIC(&BICdatos, nPruebas);
		imprimirSalida(fsalida, &BICdatos, &SACdatos, 3);
	}

	if (fsalida && (fsalida != stdout))
		fclose(fsalida);

	return 0;
}

/*--------------------------------------------------------------------------
 Obtiene los argumentos
 - Entrada:
 	* Número de argumentos
 	* Array de argumentos
 	* Numero de pruebas a realizar
 	* Puntero al fichero de salida
 	* Salida
 - Salida:
 	* -1 si ocurre algun error
 	* 0 si va bien
 --------------------------------------------------------------------------*/
int getArgs(int nArgs, char** args, int* criterio, int* nPruebas,
		char* ficherosalida, int* salida) {

	if (getCriterio(nArgs, args, criterio) != 1)
		return -1;
	if (getEntero(nArgs, args, nPruebas, "-n", 2) != 1)
		return -1;

	if (nArgs == 4)
		*salida = 0;
	else if (((*salida) = getCadena(nArgs, args, ficherosalida, "-o", 2)) != 1)
		return -1;

	return 0;
}
/*--------------------------------------------------------------------------
 Obtiene el criterio: BIC o SAC (o ambos)
 - Entrada:
 	* Número de argumentos
 	* Array de argumentos
 	* Criterio
 - Salida:
 	* -1 si ocurre algun error
 	* 0 si no se introduce ninguno
	* 1 si se introduce el criterio de manera correcta
 --------------------------------------------------------------------------*/
int getCriterio(int nArgs, char** args, int* criterio) {

	int i;
	int aux = 0;

	for (i = 1; i < nArgs; i++)
		if ((strncmp(args[i], "-S", 2) == 0) && (strlen(args[i]) == 2)) {
			if (aux)
				return -1;
			else {
				*criterio = 1;
				aux = 1;
			}
		} else if (strncmp(args[i], "-B", 2) == 0) {
			if (aux)
				return -1;
			else {
				*criterio = 2;
				aux = 1;
			}
		} else if (strncmp(args[i], "-A", 2) == 0) {
			if (aux)
				return -1;
			else {
				*criterio = 3;
				aux = 1;
			}
		}

	return aux;
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
/*--------------------------------------------------------------------------
 Calcula las estadisticas asociadas al SAC (probabilidades)
 - Entrada:
	 * Estructura de SAC con los datos necesarios
	 * Número de pruebas a realizar
 --------------------------------------------------------------------------*/
void calcularEstadisticasSAC(SAC* datos, int nPruebas) {

	unsigned short*** Sboxes = NULL;
	unsigned long int*** cambiosDeBit = NULL;
	int i, j, k;

	srand(time(NULL));

	//Preparamos la memoria para guardar los datos de diferencias que vamos obteniendo
	cambiosDeBit = guardarMemSAC();
	for (i = 0; i < NUM_S_BOXES; i++)
		for (j = 0; j < SBOX_OUTPUT_BITS; j++)
			for (k = 0; k < SBOX_INPUT_BITS; k++)
				cambiosDeBit[i][j][k] = 0;

	//Medimos los cambios que se producen
	Sboxes = guardarMemSboxes();
	for (i = 0; i < nPruebas; i++)
		pruebaSAC(Sboxes, cambiosDeBit);
	freeSboxes(Sboxes);

	//Calculamos las probabilidades dividiendo los casos favorables entre el numero total de casos
	for (i = 0; i < NUM_S_BOXES; i++)
		for (j = 0; j < SBOX_OUTPUT_BITS; j++)
			for (k = 0; k < SBOX_INPUT_BITS; k++)
				datos->condProb[i][j][k] = (cambiosDeBit[i][j][k])
						* (1. / nPruebas);

	freeMemSAC(cambiosDeBit); //Liberamos la memoria que usamos para guardar las diferencias
}
/*--------------------------------------------------------------------------
 Imprime la salida
 - Entrada:
	 * Fichero donde imprimirlo
	 * Datos del BIC a imprimir
	 * Datos del SAC a imprimir
	 * Criterio que indica qué datos imprimir
 --------------------------------------------------------------------------*/
void imprimirSalida(FILE* fsalida, BIC* datosBIC, SAC* datosSAC, int criterio) {

	int i, j, k;

	if (criterio == 1) {
		for (i = 0; i < NUM_S_BOXES; i++) {
			fprintf(fsalida, "S-box %d:\n", i + 1);
			for (j = 0; j < SBOX_OUTPUT_BITS; j++)
				for (k = 0; k < SBOX_INPUT_BITS; k++)
					fprintf(fsalida, "P(¬b%d|¬a%d) =\t%.2f\n", j + 1, k + 1,
							datosSAC->condProb[i][j][k]);
		}
	} else if (criterio == 2) {
		for (i = 0; i < NUM_S_BOXES; i++) {
			fprintf(fsalida, "S-box %d:\n", i + 1);
			for (j = 0; j < SBOX_OUTPUT_PAIRS; j++)
				for (k = 0; k < SBOX_INPUT_BITS; k++) {
					fprintf(fsalida, "P(¬b%d|¬b%d,¬a%d) =\t%.2f\n",
							OUTPUT_PAIRS[j][0] + 1, OUTPUT_PAIRS[j][1] + 1,
							k + 1, datosBIC->condProb[i][8 * j][k]);
					fprintf(fsalida, "P(¬b%d|b%d,¬a%d) =\t%.2f\n",
							OUTPUT_PAIRS[j][0] + 1, OUTPUT_PAIRS[j][1] + 1,
							k + 1, datosBIC->condProb[i][8 * j + 1][k]);
					fprintf(fsalida, "P(¬b%d|¬b%d,¬a%d) =\t%.2f\n",
							OUTPUT_PAIRS[j][1] + 1, OUTPUT_PAIRS[j][0] + 1,
							k + 1, datosBIC->condProb[i][8 * j + 4][k]);
					fprintf(fsalida, "P(¬b%d|b%d,¬a%d) =\t%.2f\n",
							OUTPUT_PAIRS[j][1] + 1, OUTPUT_PAIRS[j][0] + 1,
							k + 1, datosBIC->condProb[i][8 * j + 5][k]);
				}
		}
	} else {
		for (i = 0; i < NUM_S_BOXES; i++) {
			fprintf(fsalida, "S-box %d:\n", i + 1);
			for (j = 0; j < SBOX_OUTPUT_BITS; j++)
				for (k = 0; k < SBOX_INPUT_BITS; k++)
					fprintf(fsalida, "P(¬b%d|¬a%d) =\t%.2f\n", j + 1, k + 1,
							datosSAC->condProb[i][j][k]);
		}
		for (i = 0; i < NUM_S_BOXES; i++) {
			fprintf(fsalida, "S-box %d:\n", i + 1);
			for (j = 0; j < SBOX_OUTPUT_PAIRS; j++)
				for (k = 0; k < SBOX_INPUT_BITS; k++) {
					fprintf(fsalida, "P(¬b%d|¬b%d,¬a%d) =\t%.2f\n",
							OUTPUT_PAIRS[j][0] + 1, OUTPUT_PAIRS[j][1] + 1,
							k + 1, datosBIC->condProb[i][8 * j][k]);
					fprintf(fsalida, "P(¬b%d|b%d,¬a%d) =\t%.2f\n",
							OUTPUT_PAIRS[j][0] + 1, OUTPUT_PAIRS[j][1] + 1,
							k + 1, datosBIC->condProb[i][8 * j + 1][k]);
					fprintf(fsalida, "P(¬b%d|¬b%d,¬a%d) =\t%.2f\n",
							OUTPUT_PAIRS[j][1] + 1, OUTPUT_PAIRS[j][0] + 1,
							k + 1, datosBIC->condProb[i][8 * j + 4][k]);
					fprintf(fsalida, "P(¬b%d|b%d,¬a%d) =\t%.2f\n",
							OUTPUT_PAIRS[j][1] + 1, OUTPUT_PAIRS[j][0] + 1,
							k + 1, datosBIC->condProb[i][8 * j + 5][k]);
				}
		}

	}

}
/*--------------------------------------------------------------------------
 Calcula las estadísticas de BIC (probabilidades)
 - Entrada:
	 * Estructura BIC con los datos
	 * Numero de pruebas
 --------------------------------------------------------------------------*/
void calcularEstadisticasBIC(BIC* datos, int nPruebas) {

	unsigned short*** Sboxes = NULL;
	unsigned long int*** paresCambiados = NULL;
	int i, j, k;
	float _p1_p2, _p1p2, p1_p2, p1p2, p1, _p1, p2, _p2;

	srand(time(NULL));

	//Reservamos la memoria para ver que pares cambian
	paresCambiados = guardarMemBIC();
	for (i = 0; i < NUM_S_BOXES; i++)
		for (j = 0; j < 4 * SBOX_OUTPUT_PAIRS; j++)
			for (k = 0; k < SBOX_INPUT_BITS; k++)
				paresCambiados[i][j][k] = 0;

	//Medimos los cambios que se producen en cada prueba
	Sboxes = guardarMemSboxes();
	for (i = 0; i < nPruebas; i++)
		pruebaBIC(Sboxes, paresCambiados);
	freeSboxes(Sboxes);

	//Calculamos las probabilidades condicionadas segun la frecuencia 'joint'
	for (i = 0; i < NUM_S_BOXES; i++)
		for (j = 0; j < SBOX_OUTPUT_PAIRS; j++)
			for (k = 0; k < SBOX_INPUT_BITS; k++) {
				_p1_p2 = paresCambiados[i][4 * j][k] * (1. / nPruebas);
				_p1p2 = paresCambiados[i][4 * j + 1][k] * (1. / nPruebas);
				p1_p2 = paresCambiados[i][4 * j + 2][k] * (1. / nPruebas);
				p1p2 = paresCambiados[i][4 * j + 3][k] * (1. / nPruebas);
				_p1 = _p1_p2 + _p1p2;
				p1 = p1_p2 + p1p2;
				_p2 = _p1_p2 + p1_p2;
				p2 = _p1p2 + p1p2;

				datos->condProb[i][8 * j][k] = _p1_p2 * (1. / _p2);
				datos->condProb[i][8 * j + 1][k] = _p1p2 * (1. / p2);
				datos->condProb[i][8 * j + 2][k] = p1_p2 * (1. / _p2);
				datos->condProb[i][8 * j + 3][k] = p1p2 * (1. / p2);
				datos->condProb[i][8 * j + 4][k] = _p1_p2 * (1. / _p1);
				datos->condProb[i][8 * j + 5][k] = p1_p2 * (1. / p1);
				datos->condProb[i][8 * j + 6][k] = _p1p2 * (1. / _p1);
				datos->condProb[i][8 * j + 7][k] = p1p2 * (1. / p1);
			}
	freeMemBIC(paresCambiados);
}
/*--------------------------------------------------------------------------
 Reserva memoria para la estructura SAC
 - Salida:
	 * Estructura con la memoria reservada
 --------------------------------------------------------------------------*/
unsigned long int*** guardarMemSAC() {

	unsigned long int*** SACacc;
	int i, j;

	SACacc = (unsigned long int***) malloc(
	NUM_S_BOXES * sizeof(unsigned long int**)); //Reservamos una matriz por cada caja
	for (i = 0; i < NUM_S_BOXES; i++) {
		SACacc[i] = (unsigned long int**) malloc(
		SBOX_OUTPUT_BITS * sizeof(unsigned long int*)); //Las filas seran los bits de salida
		for (j = 0; j < SBOX_OUTPUT_BITS; j++)
			SACacc[i][j] = (unsigned long int*) malloc(
			SBOX_INPUT_BITS * sizeof(unsigned long int)); //Las columnas los bits de entrada
	}

	return SACacc;
}
/*--------------------------------------------------------------------------
 Libera la memoria de una estructura SAC
 - Entrada:
	 * Puntero a la estructura que se ha de liberar
 --------------------------------------------------------------------------*/
void freeMemSAC(unsigned long int*** SACacc) {

	int i, j;

	for (i = 0; i < NUM_S_BOXES; i++) {
		for (j = 0; j < SBOX_OUTPUT_BITS; j++)
			free(SACacc[i][j]);
		free(SACacc[i]);
	}

	free(SACacc);
	SACacc = NULL;
}

/*--------------------------------------------------------------------------
 Reserva memoria para guardar las S-boxes
 - Salida:
 * Las S-boxes generadas
 --------------------------------------------------------------------------*/
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
/*--------------------------------------------------------------------------
 Mide el SAC en DES para cada S-box
 - Entrada:
	 * S-boxes
	 * Numero de cambios en cada bit de salida cuando hay un cambio en un bit de entrada
 --------------------------------------------------------------------------*/
void pruebaSAC(unsigned short*** Sboxes, unsigned long int*** cambiosDeBit) {

	BloqueDES entrada, salida;
	BloqueDES entrada2, salida2;
	int i, j, k;

	crearBloque(&entrada, SBOX_INPUT_BITS * NUM_S_BOXES);

	//En cada iteracion cambiamos los bits y ejecutamos dos SBOX, una con
	//la entrada original y otra con la entrada transformada. Despues comparamos
	//las diferencias que se han encontrado en las dos salidas
	for (k = 0; k < SBOX_INPUT_BITS; k++) {

		copiarBloque(&entrada2, &entrada, SBOX_INPUT_BITS * NUM_S_BOXES);
		cambiarDatosEntrada(&entrada2, k);

		SBoxGeneral(&salida, &entrada, Sboxes);
		SBoxGeneral(&salida2, &entrada2, Sboxes);

		for (i = 0; i < NUM_S_BOXES; i++)
			for (j = 0; j < SBOX_OUTPUT_BITS; j++)
				if (salida2.bloque[i * SBOX_OUTPUT_BITS + j + 1]
						!= salida.bloque[i * SBOX_OUTPUT_BITS + j + 1])
					(cambiosDeBit[i][j][k])++;
	}
}
/*--------------------------------------------------------------------------
 Libera las Sboxes
 - Entrada:
	 * Sboxes a liberar
 --------------------------------------------------------------------------*/
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
/*--------------------------------------------------------------------------
 S-box del DES
 - Entrada:
	 * Bloque de entrada
	 * Bloque resultados
	 * S-box
 --------------------------------------------------------------------------*/
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
/*--------------------------------------------------------------------------
 Reserva memoria para la estructura BIC
 - Salida:
	 * Estructura con la memoria reservada
 --------------------------------------------------------------------------*/
unsigned long int*** guardarMemBIC() {

	unsigned long int*** BICacc;
	int i, j;

	BICacc = (unsigned long int***) malloc(
	NUM_S_BOXES * sizeof(unsigned long int**));
	for (i = 0; i < NUM_S_BOXES; i++) {
		BICacc[i] = (unsigned long int**) malloc(
				4 * SBOX_OUTPUT_PAIRS * sizeof(unsigned long int*));
		for (j = 0; j < 4 * SBOX_OUTPUT_PAIRS; j++)
			BICacc[i][j] = (unsigned long int*) malloc(
			SBOX_INPUT_BITS * sizeof(unsigned long int));
	}

	return BICacc;
}
/*--------------------------------------------------------------------------
 Mide las frecuencias conjuntas para cada S-box para calcular el BIC en DES
 - Entrada:
	 * S-boxes
	 * Numero de cambios en cada bit de salida cuando hay un cambio en un bit de entrada
 --------------------------------------------------------------------------*/
void pruebaBIC(unsigned short*** Sboxes, unsigned long int*** paresCambiados) {

	BloqueDES entrada, salida;
	BloqueDES entrada2, salida2;
	int i, j, k;
	int change1, change2;

	crearBloque(&entrada, SBOX_INPUT_BITS * NUM_S_BOXES);

	//Como en la prueba SAC volvemos a cambiar los bits de entrada
	//solo que ahora comparamos en pares
	for (k = 0; k < SBOX_INPUT_BITS; k++) {

		copiarBloque(&entrada2, &entrada, SBOX_INPUT_BITS * NUM_S_BOXES);
		cambiarDatosEntrada(&entrada2, k);

		SBoxGeneral(&salida, &entrada, Sboxes);
		SBoxGeneral(&salida2, &entrada2, Sboxes);

		for (i = 0; i < NUM_S_BOXES; i++)
			for (j = 0; j < SBOX_OUTPUT_PAIRS; j++) {

				
				//Comprueba si cada par de bits de salida ha cambiado
				//(Recordamos que se quiere la distribución conjunta de prob de cambio de cada par de bits de salida dado
				//un cambio en un bit de entrada, para posteriormente calcular la probabilidad condicionada de un cambio en
				//un bit de salidad dado un cambio en uno de entrada y un cambio o no cambio en otro bit de salida)
				
				if (salida2.bloque[i * SBOX_OUTPUT_BITS + OUTPUT_PAIRS[j][0] + 1]
						!= salida.bloque[i * SBOX_OUTPUT_BITS
								+ OUTPUT_PAIRS[j][0] + 1])
					change1 = 1;
				else
					change1 = 0;
				if (salida2.bloque[i * SBOX_OUTPUT_BITS + OUTPUT_PAIRS[j][1] + 1]
						!= salida.bloque[i * SBOX_OUTPUT_BITS
								+ OUTPUT_PAIRS[j][1] + 1])
					change2 = 1;
				else
					change2 = 0;

				//Segun que pares se haya cambiado aumentamos los datos de pares cambiados
				if (change1 && change2)
					(paresCambiados[i][4 * j][k])++;
				else if (change1 && !change2)
					(paresCambiados[i][4 * j + 1][k])++;
				else if (!change1 && change2)
					(paresCambiados[i][4 * j + 2][k])++;
				else
					(paresCambiados[i][4 * j + 3][k])++;
			}
	}
}
/*--------------------------------------------------------------------------
 Libera la memoria del BIC
 - Entrada:
	 * Estructura BIC a liberar
 --------------------------------------------------------------------------*/
void freeMemBIC(unsigned long int*** BICacc) {

	int i, j;

	for (i = 0; i < NUM_S_BOXES; i++) {
		for (j = 0; j < 4 * SBOX_OUTPUT_PAIRS; j++)
			free(BICacc[i][j]);
		free(BICacc[i]);
	}

	free(BICacc);
	BICacc = NULL;
}
/*--------------------------------------------------------------------------
 Obtiene las S-boxs
 - Entrada:
	 * S-boxes obtenidas
 --------------------------------------------------------------------------*/
void getSboxes(unsigned short*** Sboxes) {

	int box, r, c;

	for (box = 0; box < NUM_S_BOXES; box++)
		for (r = 0; r < ROWS_PER_SBOX; r++)
			for (c = 0; c < COLUMNS_PER_SBOX; c++)
				Sboxes[box][r][c] = S_BOXES[box][r][c];
}
/*--------------------------------------------------------------------------
 Crea un bloque
 - Entrada:
	 * Bloque a crear
	 * Tamaño del bloque
 --------------------------------------------------------------------------*/
void crearBloque(BloqueDES* b, int tamBloque) {

	int i;

	for (i = 1; i <= tamBloque; i++)
		b->bloque[i] = naleatorio(0, 1);
}
/*--------------------------------------------------------------------------
 Copia un bloque
 - Entrada:
	 * Bloque a copiar
	 * Bloque destino
	 * Tamaño del bloque
 --------------------------------------------------------------------------*/
void copiarBloque(BloqueDES* resultado, BloqueDES* entrada, int lon) {

	int i;

	for (i = 1; i <= lon; i++)
		resultado->bloque[i] = entrada->bloque[i];
}
void cambiarDatosEntrada(BloqueDES* entradaSBOX, int bit) {

	int i;
	//Cambiamos en todas las cajas el 'bit' de entrada
	for (i = bit; i < SBOX_INPUT_BITS * NUM_S_BOXES; i += SBOX_INPUT_BITS) {
		if (entradaSBOX->bloque[i + 1] == 0)
			entradaSBOX->bloque[i + 1] = 1;
		else
			entradaSBOX->bloque[i + 1] = 0;
	}
}
/*--------------------------------------------------------------------------
 Generamos un numero aleatorio entre a y b
 - Entrada:
	 * Numeros a y b, extremos del intervalo
 - Salida:
	 * Numero generado
 --------------------------------------------------------------------------*/
int naleatorio(int a, int b) {

	if (a >= b)
		return a;

	return a + (rand() % (b - a + 1));
}

