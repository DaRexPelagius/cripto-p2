#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

/*--------------------------------------------------------------------------
 CONSTANTS
 --------------------------------------------------------------------------*/
#define SBOX_OUTPUT_PAIRS 6
#define MAX_NAME_LENGTH 256
#define SAC_FLAG 1
#define BIC_FLAG 2
#define TAM_BLOQUE 64
#define NUM_ROUNDS 16
#define BITS_IN_PC1 56
#define BITS_IN_PC2 48
#define BITS_IN_IP 64
#define BITS_IN_E 48
#define BITS_IN_P 32
#define BITS_IN_SWAP 64
#define NUM_S_BOXES 8
#define ROWS_PER_SBOX 4
#define COLUMNS_PER_SBOX 16
#define SBOX_INPUT_BITS 6
#define SBOX_OUTPUT_BITS 4
#define BITS_IN_BOX 6
#define BITS_OUT_BOX 4
#define BITS_IN_BYTE 8

/*--------------------------------------------------------------------------
 DATA STRUCTURES
 --------------------------------------------------------------------------*/

//Un bloque DES. No usamos bloque[0]
typedef struct {
	uint8_t bloque[TAM_BLOQUE + 1];
} BloqueDES;

/* Parameters for the Strict Avalanche Criterion datos */
typedef struct {
	/* Probability of change in each output bit conditioned to a change in each input bit */
	float condProb[NUM_S_BOXES][SBOX_OUTPUT_BITS][SBOX_INPUT_BITS];
} SAC;

/* Parameters for the Bit Independence Criterion datos */
typedef struct {
	/* Probability of change in each output bit pair conditioned to a change in each input bit */
	float condProb[NUM_S_BOXES][8 * SBOX_OUTPUT_PAIRS][SBOX_INPUT_BITS];
} BIC;

typedef struct POLYNOMIAL {
	int degree;
	int* coefficients;
} strPolynomial;

int getArgs(int nArgs, char** args, int* criterio, int* nPruebas,
		char* ficherosalida, int* salida);
int getCriterio(int nArgs, char** args, int* criterio);
int getEntero(int nArgs, char** args, int* entero, char* modo, int longitud);
int getCadena(int nArgs, char** args, char* cadena, char* modo, int longitud);
void calcularEstadisticasBIC(BIC* datos, int nPruebas);
void calcularEstadisticasSAC(SAC* datos, int nPruebas);
void imprimirSalida(FILE* fsalida, BIC* datosBIC, SAC* datosSAC, int criterio);
unsigned long int*** guardarMemSAC();
void freeMemSAC(unsigned long int*** SACacc);
unsigned short*** guardarMemSboxes();
void pruebaSAC(unsigned short*** Sboxes, unsigned long int*** cambiosDeBit);
void freeSboxes(unsigned short*** Sboxes);
unsigned long int*** guardarMemBIC();
void pruebaBIC(unsigned short*** Sboxes, unsigned long int*** paresCambiados);
void freeMemBIC(unsigned long int*** BICacc);
void getSboxes(unsigned short*** Sboxes);
void crearBloque(BloqueDES* b, int tamBloque);
void copiarBloque(BloqueDES* resultado, BloqueDES* entrada, int lon);
void cambiarDatosEntrada(BloqueDES* entradaSBOX, int bit);
void SBoxGeneral(BloqueDES* resultado, BloqueDES* entrada,
		short unsigned int*** Sbox);
int naleatorio(int a, int b);
