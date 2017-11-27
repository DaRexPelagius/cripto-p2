#include "4a.h"

int DIRECT_AUX[BITS_IN_BYTE][BITS_IN_BYTE] = { { 1, 0, 0, 0, 1, 1, 1, 1 }, { 1,
		1, 0, 0, 0, 1, 1, 1 }, { 1, 1, 1, 0, 0, 0, 1, 1 }, { 1, 1, 1, 1, 0, 0,
		0, 1 }, { 1, 1, 1, 1, 1, 0, 0, 0 }, { 0, 1, 1, 1, 1, 1, 0, 0 }, { 0, 0,
		1, 1, 1, 1, 1, 0 }, { 0, 0, 0, 1, 1, 1, 1, 1 } };

int* DIRECT_MATRIX[BITS_IN_BYTE] = { DIRECT_AUX[0], DIRECT_AUX[1],
		DIRECT_AUX[2], DIRECT_AUX[3], DIRECT_AUX[4], DIRECT_AUX[5],
		DIRECT_AUX[6], DIRECT_AUX[7] };

int DIRECT_TRANSLATION[BITS_IN_BYTE] = { 1, 1, 0, 0, 0, 1, 1, 0 };

int IDENTITY[BITS_IN_BYTE + 1] = { 1, 1, 0, 1, 1, 0, 0, 0, 1 };

int main(int argc, char** argv) {

	int nPruebas, salida;
	char ficherosalida[MAX_NOMBRE];
	FILE* fsalida = NULL;
	NL resultados;

	//Comprobamos que al menos el numero de args es el correcto
	if ((argc != 3) && (argc != 5)) {
		printf("Numero de argumentos incorrecto.\n");
		printf("Uso: ./4a {-n numero de pruebas} [-o fichero salida]\n");
		return -1;
	}

	//Cogemos los argumentos de entrada
	if (getArgs(argc, argv, &nPruebas, ficherosalida, &salida) == -1) {
		printf("Argumentos incorrectos.\n");
		printf("Uso: ./4a {-n numero de pruebas} [-o fichero salida]\n");
		return -1;
	}

	//Comprobamos que el numero de pruebas es positivo
	if (nPruebas < 1) {
		printf("El numero de pruebas debe ser positivo.\n");
		printf("Uso: ./4a {-n numero de pruebas} [-o fichero salida]\n");
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
	//de la no linealidad de las S-boxes del AES
	calcularEstadisticas(&resultados, nPruebas);
	imprimirSalida(fsalida, &resultados);

	if (fsalida && (fsalida != stdout))
		fclose(fsalida);

	return 0;
}
/*--------------------------------------------------------------------------
 Obtiene los argumentos
 - Entrada:
	 * Número de argumentos
	 * Array de argumentos
	 * Numero de pruebas
	 * Puntero al fichero de salida
	 * Fichero de salida
 - Salida:
	 * -1 si ocurre algun error
	 * 0 si va bien
 --------------------------------------------------------------------------*/
int getArgs(int nArgs, char** args, int* nPruebas, char* ficherosalida,
		int* salida) {

	if (getEntero(nArgs, args, nPruebas, "-n", 2) != 1)
		return -1;

	if (nArgs == 3)
		*salida = 0;
	else if (((*salida) = getCadena(nArgs, args, ficherosalida, "-o", 2))
			!= 1)
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
/*--------------------------------------------------------------------------
 Calcula las estadisticas necesarias (porcentaje de coincidencias, esperanza y 
 desviacion tipica) las cuales utilizaremos para mostrar la no linealidad de AES
 - Entrada:
	 * Estructura donde guardamos los resultados
	 * Número de pruebas a realizar
 --------------------------------------------------------------------------*/
void calcularEstadisticas(NL* resultados, int nPruebas) {

	unsigned long int coincidencias[SBOX_OUTPUT_BITS + 1];
	float aux[SBOX_OUTPUT_BITS + 1];
	struct AESBOX box;
	int i;
	float temp;

	srand(time(NULL));

	//Prepramos la estructura donde anotaremos las coincidencias
	for (i = 0; i < SBOX_OUTPUT_BITS + 1; i++)
		coincidencias[i] = 0;

	getCajaAES(&box);
	//Calculamos el porcentaje de coincidencias
	for (i = 0; i < nPruebas; i++)
		coincidencias[getDatos(&box)]++;

	//Procesa los resultados
	for (i = 0; i < SBOX_OUTPUT_BITS + 1; i++)
		aux[i] = i * (1. / SBOX_OUTPUT_BITS);

	//Calculamos el porcentaje de coincidencias
	for (i = 0; i < SBOX_OUTPUT_BITS + 1; i++)
		resultados->distribucion[i] = coincidencias[i] * (1. / nPruebas);

	//Calcula la esperanza
	resultados->esperanza = 0;
	for (i = 0; i < SBOX_OUTPUT_BITS + 1; i++)
		resultados->esperanza += (aux[i]
				* resultados->distribucion[i]);

	//Calculamos la desviacion tipica
	resultados->desviacion = 0;
	for (i = 0; i < SBOX_OUTPUT_BITS + 1; i++) {
		temp = (aux[i] - resultados->esperanza);
		resultados->desviacion += temp * temp * resultados->distribucion[i];
	}
	resultados->desviacion = sqrt(resultados->desviacion);
}
/*--------------------------------------------------------------------------
 Imprime la salida
 - Entrada:
	 * Fichero donde imprimirlo
	 * Resultados a imprimir
 --------------------------------------------------------------------------*/
void imprimirSalida(FILE* fsalida, NL* resultados) {

	int j;

	fprintf(fsalida, "- Esperanza:\t%.2f\n", resultados->esperanza);
	fprintf(fsalida, "- Desviacion tipica:\t%.2f\n", resultados->desviacion);
	fprintf(fsalida, "- Porcentaje de coincidencias en los bits:\n");
	for (j = 0; j < SBOX_OUTPUT_BITS + 1; j++)
		fprintf(fsalida, "  - %d coincidencias:\t%.2f%%\n", j,
				100 * resultados->distribucion[j]);
}
/*--------------------------------------------------------------------------
 Obtiene la caja inversa a traves de la transformacion afin
 - Entrada:
	 * Puntero a la estructura
 --------------------------------------------------------------------------*/
void getCajaAES(struct AESBOX* box) {
	int i;
	for (i = 0; i < TAM_BOX; ++i)
		box->map[i] = transformacionAfin(calcularInverso(i), DIRECT_MATRIX, DIRECT_TRANSLATION);
}
/*--------------------------------------------------------------------------
 Realiza la transformacion afin dada una matriz y una traslacion
 - Entrada:
	 * Matriz
	 * Traslacion
	 * Byte
 --------------------------------------------------------------------------*/
uint8_t transformacionAfin(uint8_t b, int *matriz[], int *traslacion) {
	int bits[BITS_IN_BYTE];
	int bits2[BITS_IN_BYTE];
	int i, j;
	uint8_t resultado;

	//Pasamos el byte a bits
	for (i = 0; i < BITS_IN_BYTE; ++i)
		bits[i] = (b >> i) % 2;

	//Multiplicamos por la matriz modulo 2
	for (i = 0; i < BITS_IN_BYTE; ++i) {
		bits2[i] = 0;
		for (j = 0; j < BITS_IN_BYTE; ++j)
			bits2[i] += matriz[i][j] * bits[j];
		bits2[i] = bits2[i] % 2;
	}

	//Sumamos la translacion mod2
	for (i = 0; i < BITS_IN_BYTE; ++i)
		bits2[i] = (bits2[i] + translacion[i]) % 2;

	//Pasamos de bits a array
	resultado = 0;
	for (i = 0; i < BITS_IN_BYTE; ++i)
		resultado += bits2[i] << i;

	return resultado;
}
/*--------------------------------------------------------------------------
 Obtiene el inverso de un byte visto como un polinomio en el cuerpo de Galois
 asociado al es
 - Entrada:
	 * Byte
 --------------------------------------------------------------------------*/
uint8_t calcularInverso(uint8_t b) {
	struct POLYNOMIAL mod, p, inv;
	uint8_t ret;

	if (!b)
		return 0;

	iniPolinomio(&mod);
	iniPolinomio(&p);
	iniPolinomio(&inv);

	setPolinomio(&mod, IDENTITY, BITS_IN_BYTE);
	pasarByteAPolinomio(&p, b);

	calcularPolInverso(&inv, &p, &mod);
	ret = pasarPolinomioAByte(&inv);

	freePolinomio(&mod);
	freePolinomio(&p);
	freePolinomio(&inv);

	return ret;
}
/*--------------------------------------------------------------------------
 Obtiene los datos
 - Entrada:
	 * Puntero a la estructura AESBOX
 --------------------------------------------------------------------------*/
int getDatos(struct AESBOX* box) {

	uint8_t in1, in2, out1, out2;
	int k, n = 0, temp;

	in1 = rand() % 256;
	out1 = box->map[in1];

	in2 = rand() % 256;
	out2 = box->map[in2];

	temp = out1 ^ out2;

	//Cambio todos los bits de entrada posibles
	for (k = 0; k < SBOX_INPUT_BITS; k++) {
		n += (temp & (1 << k)) != 0;
	}

	return n;
}
/*--------------------------------------------------------------------------
 Inicializa un polinomio
 - Entrada:
	 * Puntero a la estructura del polinomio
 --------------------------------------------------------------------------*/
void iniPolinomio(struct POLYNOMIAL* p) {
	p->grado = -1;
	p->coeficientes = NULL;
}
/*--------------------------------------------------------------------------
 Le da el valor a un polinomio
 - Entrada:
	 * Polinomio
	 * Coeficientes
	 * Grado n
 --------------------------------------------------------------------------*/
void setPolinomio(struct POLYNOMIAL* p, int* coefs, int n) {
	int i;

	if (p->coeficientes)
		free(p->coeficientes);
	p->coeficientes = malloc(sizeof(int) * (n + 1));
	for (i = 0; i <= n; ++i)
		p->coeficientes[i] = coefs[i] % 2;
	p->grado = n;
}
/*--------------------------------------------------------------------------
 Convierte un byte en un polinomio
 - Entrada:
	 * Polinomio
	 * Byte
 --------------------------------------------------------------------------*/
void pasarByteAPolinomio(struct POLYNOMIAL *p, uint8_t b) {
	int i;

	free(p->coeficientes);
	p->coeficientes = malloc(sizeof(int) * BITS_IN_BYTE);
	p->grado = BITS_IN_BYTE - 1;
	for (i = 0; i < BITS_IN_BYTE; ++i)
		p->coeficientes[i] = (b >> i) % 2;
}
/*--------------------------------------------------------------------------
 Calcula el polinomio inverso
 - Entrada:
	 * Polinomio destino
	 * Modulo
	 * Polinomio a calcular el inverso
 --------------------------------------------------------------------------*/
void calcularPolInverso(struct POLYNOMIAL* dest, struct POLYNOMIAL* s,
		struct POLYNOMIAL* mod) {
	struct POLYNOMIAL q, r, p1, p2, v1, v2;
	int d1, d2;

	iniPolinomio(&q);
	iniPolinomio(&r);
	iniPolinomio(&p1);
	iniPolinomio(&p2);
	iniPolinomio(&v1);
	iniPolinomio(&v2);

	d1 = getGrado(mod);
	d2 = getGrado(s);

	copiarPolinomios(&p1, mod);
	if (d2 >= d1)
		dividirPolinomios(&q, &p2, s, mod);
	else
		copiarPolinomios(&p2, s);

	clearPolinomio(&v1);
	clearPolinomio(&v2);
	establecerCoeficienteN(&v2, 0);

	while (1) {
		dividirPolinomios(&q, &r, &p1, &p2);
		copiarPolinomios(&p1, &p2);
		copiarPolinomios(&p2, &r);

		if (getGrado(&r) < 0)
			break;

		multiplicaPolinomios(&r, &q, &v2);
		sumaPolinomios(&r, &v1, &r);
		copiarPolinomios(&v1, &v2);
		copiarPolinomios(&v2, &r);
	}

	copiarPolinomios(dest, &v2);

	freePolinomio(&q);
	freePolinomio(&r);
	freePolinomio(&p1);
	freePolinomio(&p2);
	freePolinomio(&v1);
	freePolinomio(&v2);
}
/*--------------------------------------------------------------------------
 Convierte un polinomio en un byte
 - Entrada:
	 * Polinomio
 - Salida:
	 * Byte
 --------------------------------------------------------------------------*/
uint8_t pasarPolinomioAByte(struct POLYNOMIAL* p) {
	uint8_t b = 0;
	int i;

	for (i = 0; i < BITS_IN_BYTE && i <= p->grado; ++i)
		b += (p->coeficientes[i] << i);
	return b;
}
/*--------------------------------------------------------------------------
 Libera la memoria de un polinomio
 - Entrada:
	 * Polinomio
 --------------------------------------------------------------------------*/
void freePolinomio(struct POLYNOMIAL* p) {
	p->grado = -1;
	free(p->coeficientes);
	p->coeficientes = NULL;
}
/*--------------------------------------------------------------------------
 Obtiene el grado de un polinomio
 - Entrada:
	 * Polinomio
 - Salida:
	 * Grado n
 --------------------------------------------------------------------------*/
int getGrado(struct POLYNOMIAL* p) {
	int i;
	if (p->grado < 0)
		return -1;
	for (i = p->grado; i >= 0 && !p->coeficientes[i]; i--)
		;
	return i;
}
/*--------------------------------------------------------------------------
 Multiplica polinomios
 - Entrada:
	 * Polinomio a
	 * Polinomio b
	 * Polinomio resultado
 --------------------------------------------------------------------------*/
void multiplicaPolinomios(struct POLYNOMIAL* res, struct POLYNOMIAL* a,
		struct POLYNOMIAL* b) {
	int d1 = a->grado;
	int d2 = b->grado;
	int d = d1 + d2;
	int *aux = NULL;
	int i, j;

	if (d1 < 0 || d2 < 0) {
		if (res->coeficientes)
			free(res->coeficientes);
		res->coeficientes = NULL;
		res->grado = -1;
		return;
	}

	aux = malloc(sizeof(int) * (d + 1));
	for (i = 0; i <= d; ++i) {
		aux[i] = 0;
		for (j = (d1 < i) ? d1 : i; j >= 0 && (i - j <= d2); j--)
			aux[i] += a->coeficientes[j] * b->coeficientes[i - j];
		aux[i] = aux[i] % 2;
	}

	if (res->coeficientes)
		free(res->coeficientes);
	res->coeficientes = aux;
	res->grado = d;
}
/*--------------------------------------------------------------------------
 Suma polinomios
 - Entrada:
	 * Polinomio a
	 * Polinomio b
	 * Polinomio resultado
 --------------------------------------------------------------------------*/
void sumaPolinomios(struct POLYNOMIAL* dest, struct POLYNOMIAL* s1,
		struct POLYNOMIAL* s2) {
	int* aux = NULL;
	int greater = s1->grado > s2->grado;
	int d = greater ? s1->grado : s2->grado;
	int d2 = greater ? s2->grado : s1->grado;
	int i;


	if (d == -1) {
		if (dest->coeficientes)
			free(dest->coeficientes);
		dest->coeficientes = NULL;
		dest->grado = -1;
		return;
	}

	aux = malloc(sizeof(int) * (d + 1));
	for (i = 0; i <= d2; ++i)
		aux[i] = (s1->coeficientes[i] + s2->coeficientes[i]) % 2;
	for (i = d2 + 1; i <= d; ++i)
		aux[i] = greater ? s1->coeficientes[i] : s2->coeficientes[i];

	if (dest->coeficientes)
		free(dest->coeficientes);
	dest->coeficientes = aux;
	dest->grado = d;
}
/*--------------------------------------------------------------------------
 Copia un polinomio
 - Entrada:
	 * Polinomio entrada
	 * Polinomio resultado
 --------------------------------------------------------------------------*/
void copiarPolinomios(struct POLYNOMIAL* resultado, struct POLYNOMIAL* entrada) {
	int* aux = NULL;
	int d = entrada->grado;
	int i;

	aux = malloc(sizeof(int) * (d + 1));
	for (i = 0; i <= d; ++i)
		aux[i] = entrada->coeficientes[i];

	if (resultado->coeficientes)
		free(resultado->coeficientes);
	resultado->coeficientes = aux;
	resultado->grado = d;
}
/*--------------------------------------------------------------------------
 Divide polinomios
 - Entrada:
	 * Polinomio dividendo
	 * Polinomio divisor
	 * Polinomio cociente
	 * Polinomio resto
 --------------------------------------------------------------------------*/
void dividirPolinomios(struct POLYNOMIAL* q, struct POLYNOMIAL* r, struct POLYNOMIAL* div,
		struct POLYNOMIAL* dvsr) {
	int d1, d2;
	struct POLYNOMIAL aux, qAux, rAux;

	iniPolinomio(&aux);
	iniPolinomio(&qAux);
	iniPolinomio(&rAux);

	copiarPolinomios(&rAux, div);
	clearPolinomio(&qAux);

	for (d1 = getGrado(&rAux), d2 = getGrado(dvsr); d1 >= d2;
			d1 = getGrado(&rAux), d2 = getGrado(dvsr)) {
		desplazarPolinomio(&aux, dvsr, d1 - d2);
		sumaPolinomios(&rAux, &rAux, &aux);
		establecerCoeficienteN(&qAux, d1 - d2);
	}

	copiarPolinomios(r, &rAux);
	copiarPolinomios(q, &qAux);

	freePolinomio(&aux);
	freePolinomio(&qAux);
	freePolinomio(&rAux);
}
/*--------------------------------------------------------------------------
 Borra un polinomio
 - Entrada:
	 * Polinomio
 --------------------------------------------------------------------------*/
void clearPolinomio(struct POLYNOMIAL* p) {
	int i;
	for (i = 0; i <= p->grado; ++i)
		p->coeficientes[i] = 0;
}
/*--------------------------------------------------------------------------
 Fija el valor del coeficiente n a 1
 - Entrada:
	 * Polinomio a
	 * Coeficiente n
 --------------------------------------------------------------------------*/
void establecerCoeficienteN(struct POLYNOMIAL* p, int n) {
	int d = p->grado, i;

	if (n < 0)//El polinomio tiene que tener grado positivo
		return;

	if (n <= p->grado) {
		p->coeficientes[n] = 1;//Si ya tiene un grado mayor o igual
							   //que el que queremos establecer...
		return;
	}

	//Si el grado actual es menor, relocalizamos memoria para el nuevo
	//polinomio
	p->coeficientes = realloc(p->coeficientes, sizeof(int) * (n + 1));
	for (i = d + 1; i < n; ++i)
		p->coeficientes[i] = 0;
	p->coeficientes[n] = 1;
	p->grado = n;
}
/*--------------------------------------------------------------------------
 Fija un polinomio como copia de otro polinomio multiplicado por un monomio de
 grado n
 - Entrada:
 	* Polinomio a ser asignado
  	* Polinomio origen
	* Grado del monomio
 --------------------------------------------------------------------------*/
void desplazarPolinomio(struct POLYNOMIAL* resultado, struct POLYNOMIAL* entrada, int n) {
	int* aux = NULL;
	int d = entrada->grado;
	int i;

	if (n < 0)
		return;

	aux = malloc(sizeof(int) * (d + n + 1));
	for (i = 0; i < n; ++i)
		aux[i] = 0;
	for (i = 0; i <= d; ++i)
		aux[i + n] = entrada->coeficientes[i];

	if (resultado->coeficientes)
		free(resultado->coeficientes);
	resultado->coeficientes = aux;
	resultado->grado = d + n;
}

