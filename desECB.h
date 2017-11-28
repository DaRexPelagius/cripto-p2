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
#define MAX_TEXTO 256


typedef struct {
    uint8_t k[TAM_CLAVE];
    uint8_t c[TAM_CLAVE/2];
    uint8_t d[TAM_CLAVE/2];
} DescomposicionClave;

int getArgs(int nArgs, char** args, int* modo, char* ficheroentrada,
		int* entrada, char* ficherosalida,
		int* salida);
int getModo(int nArgs, char** args, int* modo);
int getCadena(int nArgs, char** args, char* cadena, char* modo, int longitud);
void generaClave(uint8_t* clave);
void descomponerClave(uint8_t* clave, DescomposicionClave* subclaves);
void DES(uint8_t* in, uint8_t* out, DescomposicionClave* subclaves, int modo);
void aplicarPC1(uint8_t* clave, DescomposicionClave* subclaves);
void cleanDescomposicionClave(DescomposicionClave* subclaves);

