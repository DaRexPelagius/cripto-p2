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
#define ERR_N_ARGS -1
#define ERR_FLAGS -2
#define MAX_NAME_LENGTH 256

#define TAM_BLOQUE 64
#define TAM_CLAVE 64
#define NUM_ROUNDS 16
#define BITS_IN_FEISTEL 64
#define BITS_IN_KEY 56
#define BITS_IN_PC1 56
#define BITS_IN_PC2 48
#define BITS_IN_IP 64
#define BITS_IN_E 48
#define BITS_IN_P 32
#define BITS_IN_SWAP 64
#define NUM_S_BOXES 8
#define ROWS_PER_SBOX 4
#define COLUMNS_PER_SBOX 16

/*--------------------------------------------------------------------------
 DATA STRUCTURES
 --------------------------------------------------------------------------*/

//Media de diferencias en cada ronda. Aqui guardamos los dos casos,
//efecto avalancha con cambios sobre el texto y sobre la clave
typedef struct {
	float bitsCambiadosTexto[NUM_ROUNDS + 1];
	float bitsCambiadosClave[NUM_ROUNDS + 1];
} Diferencias;

//Un bloque DES. No usamos bloque[0]
typedef struct {
	uint8_t bloque[TAM_BLOQUE + 1];
} BloqueDES;

int getArgs(int nArgs, char** args, int* nPruebas, char* ficherosalida,
		int* salida);
int getEntero(int nArgs, char** args, int* entero, char* modo, int longitud);
int getCadena(int nArgs, char** args, char* cadena, char* modo, int longitud);
void calcularEstadisticas(Diferencias* dif, int nTests);
void imprimirSalida(FILE* fsalida, Diferencias* dif);
void pruebaTextoCambiado(BloqueDES* entrada, BloqueDES* clave,
		unsigned long int* bitChangesAcc, short unsigned int*** Sboxes);
void pruebaClaveCambiada(BloqueDES* entrada, BloqueDES* clave,
		unsigned long int* bitChangesAcc, short unsigned int*** Sboxes);
void crearClave(BloqueDES* clave);
void crearBloque(BloqueDES* b, int tamBloque);
void copiarBloque(BloqueDES* resultado, BloqueDES* entrada, int lon);
void cambioBit(BloqueDES* b, int tamBloque);
int calculaDiferencias(BloqueDES* b1, BloqueDES* b2, int tamBloque);
void permInicial(BloqueDES* resultado, BloqueDES* entrada);
void getParteIzquierda(BloqueDES* semiBlock, BloqueDES* fullBlock);
void getParteDerecha(BloqueDES* semiBlock, BloqueDES* fullBlock);
void aplicarPC1(BloqueDES* resultado, BloqueDES* entrada);
void aplicarLCS(BloqueDES* resultado, BloqueDES* entrada, int nRonda, int modo);
void aplicarPC2(BloqueDES* resultado, BloqueDES* entrada);
void aplicarDES(BloqueDES* newLeft, BloqueDES* newRight, BloqueDES* oldLeft,
		BloqueDES* oldRight, BloqueDES* clave, int nRonda,
		short unsigned int*** Sbox);
void aplicarMatriz(BloqueDES* resultado, BloqueDES* old,
		const unsigned short* indices, int length);
void desplazamientoIzq(BloqueDES* resultado, BloqueDES* old, int shift);
void desplazamientoDer(BloqueDES* resultado, BloqueDES* old, int shift);
void expansion(BloqueDES* resultado, BloqueDES* old);
void xorDES(BloqueDES* resultado, BloqueDES* old1, BloqueDES* old2, int length);
void SBoxGeneral(BloqueDES* resultado, BloqueDES* old,
		short unsigned int*** Sbox);
void aplicarP(BloqueDES* resultado, BloqueDES* old);
int naleatorio(int a, int b);
unsigned short*** guardarMemSboxes();
void freeSboxes(unsigned short*** Sboxes);
void freeSboxes(unsigned short*** Sboxes);
void getSboxes(unsigned short*** Sboxes);

