/*--------------------------------------------------------------------------
LIBRARIES
--------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <time.h>


/*--------------------------------------------------------------------------
CONSTANTS
--------------------------------------------------------------------------*/
#define MAX_BUFFER 100
#define SIZE_OF_CHAR 8
#define MAX_TEXTO 10000
#define MAX_NOMBRE 256
#define PADDING_CHAR '.'
#define FORMATO_HEX 'h'
#define FORMATO_BIN 'b'
#define FORMATO_ASCII 'a'
#define SAC_FLAG 1
#define BIC_FLAG 2
#define TAM_BLOQUE 64
#define TAM_CLAVE 64
#define RONDAS 16
#define BITS_IN_FEISTEL 64
#define BITS_IN_KEY 56
#define BITS_IN_PC1 56
#define BITS_IN_PC2 48
#define BITS_MEDIOBLOQUE BITS_IN_KEY/2
#define BITS_IN_IP 64
#define BITS_IN_E 48
#define BITS_IN_P 32
#define BITS_IN_SWAP 64
#define NUM_S_BOXES 8
#define ROWS_PER_SBOX 4
#define COLUMNS_PER_SBOX 16
#define SBOX_INPUT_BITS 6
#define SBOX_OUTPUT_BITS 4


/*--------------------------------------------------------------------------
DATA STRUCTURES
--------------------------------------------------------------------------*/

//Bloque de DES
typedef struct {
	uint8_t bloque[TAM_BLOQUE + 1];//bloque[0] no lo usamos
} Bloque;

//Parametros de CBC
typedef struct {
	Bloque clave;
	int tamBloque;
} ECB;


int getArgs(int nArgs, char** args, int* modo, char* clave, char* formatoEntrada, char* formatoSalida, char* ficheroentrada, int* entrada, char* ficherosalida, int* salida);
int getEntero(int nArgs, char** args, int* entero, char* flag, int flagLength);
int getCadena(int nArgs, char** args, char* string, char* flag, int flagLength);
int getFormato(int nArgs, char** args, char* formato, char* flag, int flagLength);
int getModo(int nArgs, char** args, int* modo);
void imprimirSalida(FILE* fsalida, char formato, char* texto, int lon);
int prepararECB(char* clave, int tamBloque, ECB* ecb);
void padding(char* texto, int* lon, int number, char padChar);
void imprimirSalida(FILE* fsalida, char formato, char* texto, int lon);
int leerEntrada(FILE* fentrada, char formato, char* texto, int maxLon);
void imprimirClave(FILE* fsalida, Bloque* clave);
void modoECB(int modo, char* textoplano, char* textocifrado, int lon, ECB* ecb);
int esValida(Bloque* clave);
int hex2Bloque(Bloque* newbloque, char* string, int length);
void char2Hex(char* hex, uint8_t c);
void newClave(Bloque* clave);
int naleatorio(int a, int b);
void imprimirBloqueHexadecimal(FILE* fsalida, Bloque* b, int tamBloque, char* texto);
int comprobarHex(char hex);
void hex2Char(uint8_t* c, char* hex);
void texto2Bloque(Bloque* b, char* texto, int tamBloque);
void fECB(int flag, Bloque* bloqueTextoPlano, Bloque* bloqueTextoCifrado, ECB* ecb);
void DES(Bloque* output, Bloque* input, Bloque* clave, int flag);
void bloque2Texto(char* texto, Bloque* b, int tamBloque);
void char2Bin(uint8_t* bin, uint8_t c);
void bin2Hex(char* c, uint8_t* bin);
void hex2Bin(uint8_t* bin, char hex);
void xorDES(Bloque* resultado, Bloque* old1, Bloque* old2, int length);
void bin2Char(uint8_t* c, uint8_t* bin);
void permArranque(Bloque* resultado, Bloque* entrada);
void getMitadIzq(Bloque* resultado, Bloque* entrada);
void getMitadDer(Bloque* resultado, Bloque* entrada);
void permutacionClave1(Bloque* resultado, Bloque* entrada);
void permutacionClave2(Bloque* resultado, Bloque* entrada);
void LCS(Bloque* resultado, Bloque* entrada, int nRonda, int modo);
void shiftLeftDES(Bloque* resultado, Bloque* entrada, int shift);
void shiftRightDES(Bloque* resultado, Bloque* entrada, int shift);
void rondaDES(Bloque* izqSalida, Bloque* derSalida, Bloque* izqEntrada, Bloque* derEntrada, Bloque* clave);
void copiarBloque(Bloque* resultado, Bloque* entrada, int length);
void unirBloques(Bloque* fullbloque, Bloque* left, Bloque* right);
void swap(Bloque* resultado, Bloque* entrada);
void invPermArranque(Bloque* resultado, Bloque* entrada);
void permutacion(Bloque* resultado, Bloque* entrada, const unsigned short* indices, int length);
void expansion(Bloque* resultado, Bloque* entrada);
void SBox(Bloque* resultado, Bloque* entrada);
void permutacionF_DES(Bloque* resultado, Bloque* entrada);
