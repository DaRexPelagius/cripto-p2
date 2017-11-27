#include "4b.h"

/*--------------------------------------------------------------------------
 COEFFICIENTS FOR AFFINE TRANSFORMATIONS
 --------------------------------------------------------------------------*/

int DIRECT_AUX[BITS_IN_BYTE][BITS_IN_BYTE] = { { 1, 0, 0, 0, 1, 1, 1, 1 }, { 1,
		1, 0, 0, 0, 1, 1, 1 }, { 1, 1, 1, 0, 0, 0, 1, 1 }, { 1, 1, 1, 1, 0, 0,
		0, 1 }, { 1, 1, 1, 1, 1, 0, 0, 0 }, { 0, 1, 1, 1, 1, 1, 0, 0 }, { 0, 0,
		1, 1, 1, 1, 1, 0 }, { 0, 0, 0, 1, 1, 1, 1, 1 } };

int* DIRECT_MATRIX[BITS_IN_BYTE] = { DIRECT_AUX[0], DIRECT_AUX[1],
		DIRECT_AUX[2], DIRECT_AUX[3], DIRECT_AUX[4], DIRECT_AUX[5],
		DIRECT_AUX[6], DIRECT_AUX[7] };

int DIRECT_TRANSLATION[BITS_IN_BYTE] = { 1, 1, 0, 0, 0, 1, 1, 0 };

int INVERSE_AUX[BITS_IN_BYTE][BITS_IN_BYTE] = { { 0, 0, 1, 0, 0, 1, 0, 1 }, { 1,
		0, 0, 1, 0, 0, 1, 0 }, { 0, 1, 0, 0, 1, 0, 0, 1 }, { 1, 0, 1, 0, 0, 1,
		0, 0 }, { 0, 1, 0, 1, 0, 0, 1, 0 }, { 0, 0, 1, 0, 1, 0, 0, 1 }, { 1, 0,
		0, 1, 0, 1, 0, 0 }, { 0, 1, 0, 0, 1, 0, 1, 0 } };

int* INVERSE_MATRIX[BITS_IN_BYTE] = { INVERSE_AUX[0], INVERSE_AUX[1],
		INVERSE_AUX[2], INVERSE_AUX[3], INVERSE_AUX[4], INVERSE_AUX[5],
		INVERSE_AUX[6], INVERSE_AUX[7] };

int INVERSE_TRANSLATION[BITS_IN_BYTE] = { 1, 0, 1, 0, 0, 0, 0, 0 };

int IDENTITY[BITS_IN_BYTE + 1] = { 1, 1, 0, 1, 1, 0, 0, 0, 1 };

int main(int argc, char** argv) {

	int modo;
	int salida;
	char ficherosalida[MAX_NOMBRE];
	FILE* fsalida = NULL;
	struct AESBOX box;

	//Comprobamos que al menos el numero de args es el correcto
	if ((argc != 3) && (argc != 5)) {
		printf("Numero de argumentos incorrecto.\n");
		printf("Uso: ./4b {-C caja directa | -D caja inversa} [-o fichero salida]\n");
		return -1;
	}

	//Cogemos los argumentos de entrada
	if (getArgs(argc,argv,&modo,ficherosalida,&salida) == -1) {
		printf("Argumentos incorrectos.\n");
		printf("Uso: ./4b {-C caja directa | -D caja inversa} [-o fichero salida]\n");
		return -1;
	}


	if (salida) {
		if ((fsalida = fopen(ficherosalida, "w")) == NULL) {
			fprintf(stderr, "Error al abrir el fichero: %s .\n", ficherosalida);
			return -1;
		}
	} else
		fsalida = stdout;

	//Calculamos la caja
	if (modo == 1)
		getCajaAES(&box);
	else
		getCajaAESInversa(&box);
	imprimirSalida(fsalida, &box);

	fclose(fsalida);
	return 0;
}

int getArgs(int nArgs, char** args, int* flag, char* ficherosalida,
		int* salida) {

	if (getModo(nArgs, args, flag) != 1)
		return -1;

	if (nArgs == 2) {
		*salida = 0;
	} else if (((*salida) = getCadena(nArgs, args, ficherosalida, "-o", 2))
			== -1)
		return -1;

	return 0;
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

void getCajaAES(struct AESBOX* box) {
	int i;
	for (i = 0; i < TAM_BOX; ++i)
		box->map[i] = transformacionAfin(calcularInverso(i), DIRECT_MATRIX,
				DIRECT_TRANSLATION);
}

void getCajaAESInversa(struct AESBOX* box) {
	int i;
	for (i = 0; i < TAM_BOX; ++i)
		box->map[i] = calcularInverso(
				transformacionAfin(i, INVERSE_MATRIX, INVERSE_TRANSLATION));

}

void imprimirSalida(FILE* f, struct AESBOX* box) {
	int i, j;

	for (i = 0; i < 16; ++i) {
		for (j = 0; j < 16; ++j)
			fprintf(f, "%02X\t", box->map[(i << 4) + j]);
		fprintf(f, "\n");
	}
}

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

uint8_t transformacionAfin(uint8_t b, int *matriz[], int *translacion) {
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

void iniPolinomio(struct POLYNOMIAL* p) {
	p->grado = -1;
	p->coeficientes = NULL;
}

void setPolinomio(struct POLYNOMIAL* p, int* coefs, int n) {
	int i;

	if (p->coeficientes)
		free(p->coeficientes);
	p->coeficientes = malloc(sizeof(int) * (n + 1));
	for (i = 0; i <= n; ++i)
		p->coeficientes[i] = coefs[i] % 2;
	p->grado = n;
}

void pasarByteAPolinomio(struct POLYNOMIAL *p, uint8_t b) {
	int i;

	free(p->coeficientes);
	p->coeficientes = malloc(sizeof(int) * BITS_IN_BYTE);
	p->grado = BITS_IN_BYTE - 1;
	for (i = 0; i < BITS_IN_BYTE; ++i)
		p->coeficientes[i] = (b >> i) % 2;
}

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

uint8_t pasarPolinomioAByte(struct POLYNOMIAL* p) {
	uint8_t b = 0;
	int i;

	for (i = 0; i < BITS_IN_BYTE && i <= p->grado; ++i)
		b += (p->coeficientes[i] << i);
	return b;
}

void freePolinomio(struct POLYNOMIAL* p) {
	p->grado = -1;
	free(p->coeficientes);
	p->coeficientes = NULL;
}

int getGrado(struct POLYNOMIAL* p) {
	int i;
	if (p->grado < 0)
		return -1;
	for (i = p->grado; i >= 0 && !p->coeficientes[i]; i--)
		;
	return i;
}

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

void clearPolinomio(struct POLYNOMIAL* p) {
	int i;
	for (i = 0; i <= p->grado; ++i)
		p->coeficientes[i] = 0;
}

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

void desplazarPolinomio(struct POLYNOMIAL* resultado,
		struct POLYNOMIAL* entrada, int n) {
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

