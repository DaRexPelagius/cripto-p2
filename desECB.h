#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define CIFRAR 1
#define DESCIFRAR 0
#define TAM_CLAVE 8
#define BITS_IN_PC1 56
#define BITS_IN_PC2 48
#define ROUNDS 16
#define BITS_IN_IP 64
#define BITS_IN_E 48
#define BITS_IN_P 32
#define NUM_S_BOXES 8
#define ROWS_PER_SBOX 4
#define COLUMNS_PER_SBOX 16
#define MAX_NOMBRE 256


typedef struct {
    uint8_t k[TAM_CLAVE];
    uint8_t c[TAM_CLAVE/2];
    uint8_t d[TAM_CLAVE/2];
} subclave;

int getArgs(int nArgs, char** args, int* modo,uint8_t* clave, char* ficheroentrada,
		int* entrada, char* ficherosalida,
		int* salida);
int getModo(int nArgs, char** args, int* modo);
void getClave(int nArgs, char** args, uint8_t* clave);
int getCadena(int nArgs, char** args, char* cadena, char* modo, int longitud);
void generaClave(uint8_t* clave);
void generaSubClavesDES(uint8_t* clave, subclave* subclaves);
void DES(uint8_t* in, uint8_t* out, subclave* subclaves, int modo);
uint8_t ** DES_Avalancha(uint8_t* in, uint8_t* out, subclave* subclaves, int modo);
void DES_CBC(uint8_t* in, uint8_t* out, subclave* subclaves,uint8_t * chain, int modo);
