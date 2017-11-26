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
#define MAX_TEXTO 10000
#define MAX_NOMBRE 256
#define ENCRYPT_FLAG 1
#define DECRYPT_FLAG 2
#define DES_BLOCK_SIZE 64
#define DES_KEY_SIZE 64
#define NUM_ROUNDS 16
#define BITS_IN_FEISTEL 64
#define BITS_IN_KEY 56
#define BITS_IN_PC1 56
#define BITS_IN_PC2 48
#define BITS_IN_HALFBLOCK BITS_IN_KEY/2
#define BITS_IN_IP 64
#define BITS_IN_E 48
#define BITS_IN_P 32
#define BITS_IN_SWAP 64
#define NUM_S_BOXES 8
#define ROWS_PER_SBOX 4
#define COLUMNS_PER_SBOX 16
#define SBOX_INPUT_BITS 6
#define SBOX_OUTPUT_BITS 4
#define SBOX_OUTPUT_PAIRS 6
#define BITS_IN_BOX 6
#define BITS_OUT_BOX 4
#define BITS_IN_BYTE 8

//Estructura para guardar algunos datos que concluimos de las pruebas
typedef struct {
	/* Expectation of linearity of each S-box */
	float expectation[NUM_S_BOXES]; //
	/* Standard desviacion of linearity of each S-box */
	float desviacion[NUM_S_BOXES]; //Desviacion tipica de la linealidad de cada caja
	float coincidencias[NUM_S_BOXES * (SBOX_OUTPUT_BITS + 1)]; //Porcentaje de coincidencias
} NL;

typedef struct {
	uint8_t bloque[DES_BLOCK_SIZE + 1]; /* block[0] is never used */
} BloqueDES;

int getArgs(int nArgs, char** args, int* nPruebas, char* ficherosalida,
		int* salida);
int getCadena(int nArgs, char** args, char* cadena, char* modo, int longitud);
int getEntero(int nArgs, char** args, int* entero, char* modo, int longitud);
void imprimirSalida(FILE* outputFile, NL* resultados);
void calcularEstadisticas(NL* resultados, int nPruebas);
unsigned short*** guardarMemSboxes();
void freeSboxes(unsigned short*** Sboxes);
void getDatos(unsigned short*** Sboxes, unsigned long int* coincidencias);
unsigned short*** getresultadosRandomSboxes();
unsigned short*** getresultadosSboxes();
void getSboxes(unsigned short*** Sboxes);
void nuevoBloque(BloqueDES* b, int blockSize);
void xorDES(BloqueDES* resultados, BloqueDES* a, BloqueDES* b, int longitud);
void SBoxGeneral(BloqueDES* resultados, BloqueDES* old,
		short unsigned int*** Sbox);
int naleatorio(int a, int b);
