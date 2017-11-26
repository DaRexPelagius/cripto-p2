#include "NL_DES.h"
#include <math.h>

int MATRIX_AUX2[ROWS_PER_SBOX][BITS_OUT_BOX][BITS_OUT_BOX] = { { { 1, 1, 1, 0 },
		{ 1, 1, 0, 0 }, { 1, 0, 0, 0 }, { 1, 0, 0, 1 } }, { { 1, 1, 0, 1 }, { 0,
		0, 0, 1 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 } }, { { 1, 1, 0, 0 }, { 0, 1,
		0, 0 }, { 1, 0, 1, 1 }, { 0, 0, 0, 1 } }, { { 1, 1, 1, 1 },
		{ 1, 1, 1, 0 }, { 1, 1, 0, 0 }, { 1, 0, 0, 0 } } };

int* MATRIX_AUX1[ROWS_PER_SBOX][BITS_OUT_BOX] = { { MATRIX_AUX2[0][0],
		MATRIX_AUX2[0][1], MATRIX_AUX2[0][2], MATRIX_AUX2[0][3] }, {
		MATRIX_AUX2[1][0], MATRIX_AUX2[1][1], MATRIX_AUX2[1][2],
		MATRIX_AUX2[1][3] }, { MATRIX_AUX2[2][0], MATRIX_AUX2[2][1],
		MATRIX_AUX2[2][2], MATRIX_AUX2[2][3] }, { MATRIX_AUX2[3][0],
		MATRIX_AUX2[3][1], MATRIX_AUX2[3][2], MATRIX_AUX2[3][3] }, };

int** MATRIX[ROWS_PER_SBOX] = { MATRIX_AUX1[0], MATRIX_AUX1[1], MATRIX_AUX1[2],
		MATRIX_AUX1[3] };

int TRANSLATION[BITS_OUT_BOX] = { 1, 1, 0, 0 };

int POLS_AUX[2][BITS_OUT_BOX + 1] = { { 1, 1, 1, 1, 1 }, { 1, 1, 0, 0, 1 }, };

int* POLS[2] = { POLS_AUX[0], POLS_AUX[1] };

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
	char ficherosalida[MAX_NOMBRE];
	FILE* fsalida = NULL;
	NL resultados;

	//Comprobamos que al menos el numero de args es el correcto
	if ((argc != 3) && (argc != 5)) {
		printf("Numero de argumentos incorrecto.\n");
		printf("Uso: ./NL_DES {-n numero de pruebas} [-o fichero salida]\n");
		return -1;
	}

	//Cogemos los argumentos de entrada
	if (getArgs(argc, argv, &nPruebas, ficherosalida, &salida) == -1) {
		printf("Argumentos incorrectos.\n");
		printf("Uso: ./NL_DES {-n numero de pruebas} [-o fichero salida]\n");
		return -1;
	}

	//Comprobamos que el numero de pruebas es positivo
	if (nPruebas < 1) {
		printf("El numero de pruebas debe ser positivo.\n");
		printf("Uso: ./NL_DES {-n numero de pruebas} [-o fichero salida]\n");
		return -1;
	}

	if (salida) {
		if ((fsalida = fopen(ficherosalida, "w")) == NULL) {
			fprintf(stderr, "Error al abrir el fichero: %s .\n", ficherosalida);
			return -1;
		}
	} else
		fsalida = stdout;

	//Procedemos a calcular las estadisticas que podemos obtener
	//de la no linealidad de las S-boxes del DES
	calcularEstadisticas(&resultados, nPruebas);
	imprimirSalida(fsalida, &resultados);

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

void calcularEstadisticas(NL* resultados, int nPruebas) {

	unsigned short*** Sboxes = NULL;
	unsigned long int coincidencias[NUM_S_BOXES * (SBOX_OUTPUT_BITS + 1)];
	int i, j;
	float aux[SBOX_OUTPUT_BITS + 1];
	float temp;

	srand(time(NULL));

	//Prepramos la estructura donde anotaremos las coincidencias
	for (i = 0; i < NUM_S_BOXES * (SBOX_OUTPUT_BITS + 1); i++)
		coincidencias[i] = 0;

	//Calculamos las distribuciones de linealidad
	Sboxes = guardarMemSboxes();	//Preparamos las cajas
	for (i = 0; i < nPruebas; i++)
		getDatos(Sboxes, coincidencias); //Calculamos coincidencias
	freeSboxes(Sboxes);

	/* Computes the resultados */ // ¿?
	for (j = 0; j < SBOX_OUTPUT_BITS + 1; j++)
		aux[j] = j * (1. / SBOX_OUTPUT_BITS);

	for (i = 0; i < NUM_S_BOXES; i++) {
		//Calculamos el porcentaje de coincidencias
		for (j = 0; j < SBOX_OUTPUT_BITS + 1; j++)
			resultados->coincidencias[(SBOX_OUTPUT_BITS + 1) * i + j] =
					coincidencias[(SBOX_OUTPUT_BITS + 1) * i + j]
							* (1. / nPruebas);

		/* Expectation */ // ¿?
		resultados->expectation[i] = 0;
		for (j = 0; j < SBOX_OUTPUT_BITS + 1; j++)
			resultados->expectation[i] +=
					(aux[j]
							* resultados->coincidencias[(SBOX_OUTPUT_BITS + 1)
									* i + j]);

		//Calculamos la desviacion tipica
		resultados->desviacion[i] = 0;
		for (j = 0; j < SBOX_OUTPUT_BITS + 1; j++) {
			temp = (aux[j] - resultados->expectation[i]);
			resultados->desviacion[i] += temp * temp
					* resultados->coincidencias[(SBOX_OUTPUT_BITS + 1) * i + j];
		}
		resultados->desviacion[i] = sqrt(resultados->desviacion[i]); //Nos falta aplicar la raiz cuadrada
	}
}

void imprimirSalida(FILE* fsalida, NL* resultados) {

	int i, j;

	for (i = 0; i < NUM_S_BOXES; i++) {
		fprintf(fsalida, "Datos sobre S-box numero %d:\n", i + 1);
		fprintf(fsalida, "- Expectation:\t%.2f\n", resultados->expectation[i]);
		fprintf(fsalida, "- Desviacion tipica:\t%.2f\n",
				resultados->desviacion[i]);
		fprintf(fsalida, "- Porcentaje de coincidencias en los bits:\n");
		for (j = 0; j < SBOX_OUTPUT_BITS + 1; j++)
			fprintf(fsalida, "  - %d coincidencias:\t%.2f%%\n", j,
					100
							* resultados->coincidencias[(SBOX_OUTPUT_BITS + 1)
									* i + j]);
	}
}

unsigned short*** guardarMemSboxes() {

	unsigned short*** Sboxes;
	int i, j;
	printf("HOLA [AAAAAA]\n");
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

void getDatos(unsigned short*** Sboxes, unsigned long int* coincidencias) {

	BloqueDES x, y, S_xXORy, S_xXORS_y;
	BloqueDES xXORy, S_x, outputY;
	int i, j;
	int coincidences;

	//Preparamos un par de bloques nuevos par aprobar la linealidad
	//de entrada para las cajas
	nuevoBloque(&x, SBOX_INPUT_BITS * NUM_S_BOXES);
	nuevoBloque(&y, SBOX_INPUT_BITS * NUM_S_BOXES);

	//Aplicamos la suma en la caja S(x+y)
	xorDES(&xXORy, &x, &y, SBOX_INPUT_BITS * NUM_S_BOXES);	//x+y
	SBoxGeneral(&S_xXORy, &xXORy, Sboxes);	//S(x+y)

	//Aplicamos la suma fuera de la caja  S(x)+S(y)
	SBoxGeneral(&S_x, &x, Sboxes);	//S(x)
	SBoxGeneral(&outputY, &y, Sboxes);	//S(y)
	xorDES(&S_xXORS_y, &S_x, &outputY, SBOX_INPUT_BITS * NUM_S_BOXES);//S(x)+S(y)

	for (i = 0; i < NUM_S_BOXES; i++) {
		coincidences = 0;
		for (j = 0; j < SBOX_OUTPUT_BITS; j++) {
			if (S_xXORy.bloque[i * SBOX_OUTPUT_BITS + j + 1]
					!= S_xXORS_y.bloque[i * SBOX_OUTPUT_BITS + j + 1])
				coincidences++;
		}
		(coincidencias[(SBOX_OUTPUT_BITS + 1) * i + coincidences])++;
	}
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

void nuevoBloque(BloqueDES* b, int tamBloque) {

	int i;

	for (i = 1; i <= tamBloque; i++)
		b->bloque[i] = naleatorio(0, 1);
}

//a xor b
void xorDES(BloqueDES* resultados, BloqueDES* a, BloqueDES* b, int longitud) {

	int i;

	for (i = 1; i <= longitud; i++)
		resultados->bloque[i] = (a->bloque[i] != b->bloque[i]);
}

//Aplicamos la caja
void SBoxGeneral(BloqueDES* resultados, BloqueDES* entrada,
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
		resultados->bloque[1 + i * 4 + 3] = valor % 2;
		resultados->bloque[1 + i * 4 + 2] = (valor / 2) % 2;
		resultados->bloque[1 + i * 4 + 1] = (valor / 4) % 2;
		resultados->bloque[1 + i * 4] = (valor / 8) % 2;
	}
}


int naleatorio(int a, int b) {

	if (a >= b)
		return a;

	return a + (rand() % (b - a + 1));
}


