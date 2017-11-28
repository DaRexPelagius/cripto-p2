#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_NOMBRE 256
#define TAM_BOX 256
#define BITS_IN_BYTE 8



typedef struct POLYNOMIAL {
	int grado;
	int* coeficientes;
} strPolynomial;

typedef struct AESBOX {
	uint8_t map[TAM_BOX];
} strAesBox;

int getArgs(int nArgs, char** args, int* modo, char* ficherosalida,
		int* salida);
int getModo(int nArgs, char** args, int* flag);
int getCadena(int nArgs, char** args, char* string, char* flag, int flagLength);
void getCajaAES(struct AESBOX* box);
void getCajaAESInversa(struct AESBOX* box);
void imprimirSalida(FILE* f, struct AESBOX* box);
uint8_t calcularInverso(uint8_t b);
uint8_t transformacionAfin(uint8_t b, int *matriz[], int *translacion);
void iniPolinomio(struct POLYNOMIAL* p);
void pasarByteAPolinomio(struct POLYNOMIAL *p, uint8_t b);
void setPolinomio(struct POLYNOMIAL* p, int* coefs, int n);
void calcularPolInverso(struct POLYNOMIAL* dest, struct POLYNOMIAL* s,
		struct POLYNOMIAL* mod);
uint8_t pasarPolinomioAByte(struct POLYNOMIAL* p);
void freePolinomio(struct POLYNOMIAL* p);
int getGrado(struct POLYNOMIAL* p);
void copiarPolinomios(struct POLYNOMIAL* resultado, struct POLYNOMIAL* entrada);
void dividirPolinomios(struct POLYNOMIAL* q, struct POLYNOMIAL* r, struct POLYNOMIAL* div,
		struct POLYNOMIAL* dvsr);
void clearPolinomio(struct POLYNOMIAL* p);
void establecerCoeficienteN(struct POLYNOMIAL* p, int n);
void multiplicaPolinomios(struct POLYNOMIAL* res, struct POLYNOMIAL* a,
		struct POLYNOMIAL* b);
void sumaPolinomios(struct POLYNOMIAL* dest, struct POLYNOMIAL* s1,
		struct POLYNOMIAL* s2);
void desplazarPolinomio(struct POLYNOMIAL* resultado,
		struct POLYNOMIAL* entrada, int n);
