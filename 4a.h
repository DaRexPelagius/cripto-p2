#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

/*--------------------------------------------------------------------------
 CONSTANTS
 --------------------------------------------------------------------------*/
#define MAX_NOMBRE 256
#define SBOX_OUTPUT_BITS 8
#define SBOX_INPUT_BITS 8
#define TAM_BOX 256
#define BITS_IN_BYTE 8
#define BLOCK_WIDTH 4
#define BYTES_IN_WORD 4
#define AES_ROUNDS 10

/*--------------------------------------------------------------------------
 DATA STRUCTURES
 --------------------------------------------------------------------------*/

typedef struct AESBLOCK {
	uint8_t block[BLOCK_WIDTH][BYTES_IN_WORD];
} strAesBlock;

/* Parameters for the Non-Linearity statistics, in the range [0,1] (0 if 100% linear, 1 if 100% non-linear) */
typedef struct {
	/* esperanza of linearity */
	float esperanza;
	/* Standard desviacion of linearity */
	float desviacion;
	/* distribucion (percentage) of the number of bit coincidences */
	float distribucion[SBOX_OUTPUT_BITS + 1];
} NL;

typedef struct AESEXP {
	uint8_t map[(AES_ROUNDS + 1) * BLOCK_WIDTH][BYTES_IN_WORD];
} strAesExp;

typedef struct AESBOX {
	uint8_t map[TAM_BOX];
} strAesBox;

typedef struct POLYNOMIAL {
	int grado;
	int* coeficientes;
} strPolynomial;

int getArgs(int nArgs, char** args, int*, char* ficherosalida, int* salida);
void printUsage(char* message);
int getEntero(int nArgs, char** args, int* entero, char* modo, int longitud);
int getCadena(int nArgs, char** args, char* cadena, char* modo, int longitud);
void imprimirSalida(FILE* fsalida, NL* statistics);
void calcularEstadisticas(NL* statistics, int);
void getCajaAES(struct AESBOX* box);
uint8_t transformacionAfin(uint8_t b, int *matrix[], int *translation);
uint8_t calcularInverso(uint8_t b);
int getDatos(struct AESBOX* box);
void iniPolinomio(struct POLYNOMIAL* p);
void setPolinomio(struct POLYNOMIAL* p, int* coefs, int n);
void pasarByteAPolinomio(struct POLYNOMIAL *p, uint8_t b);
void calcularPolInverso(struct POLYNOMIAL* dest, struct POLYNOMIAL* s,
		struct POLYNOMIAL* mod);
uint8_t pasarPolinomioAByte(struct POLYNOMIAL* p);
void freePolinomio(struct POLYNOMIAL* p);
int getGrado(struct POLYNOMIAL* p);
void sumaPolinomios(struct POLYNOMIAL* resultado, struct POLYNOMIAL* a,
		struct POLYNOMIAL* b);
void multiplicaPolinomios(struct POLYNOMIAL* dest, struct POLYNOMIAL* s1,
		struct POLYNOMIAL* s2);
void copiarPolinomios(struct POLYNOMIAL* resultado, struct POLYNOMIAL* entrada);
void dividirPolinomios(struct POLYNOMIAL* q, struct POLYNOMIAL* r, struct POLYNOMIAL* div,
		struct POLYNOMIAL* dvsr);
void clearPolinomio(struct POLYNOMIAL* p);
void establecerCoeficienteN(struct POLYNOMIAL* dest, int n);
void desplazarPolinomio(struct POLYNOMIAL* dest, struct POLYNOMIAL* s, int n);
