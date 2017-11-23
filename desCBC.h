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
	uint8_t bloque[TAM_BLOQUE + 1];
} Bloque;

//Parametros de CBC
typedef struct {
	Bloque clave;
	Bloque SR;
	Bloque IV;
	int lenVecIni;
	int tamBloque;
} CBC;


int getArgs(int nArgs, char** args, int* flag, char* clave, int* tamBloque, char* bufferVecIni, int* lenVecIni, char* formatoEntrada, char* formatoSalida, char* ficheroentrada, int* entrada, char* ficherosalida, int* salida);
int getEntero(int nArgs, char** args, int* entero, char* flag, int flagLength);
int getCadena(int nArgs, char** args, char* string, char* flag, int flagLength);
int getFormato(int nArgs, char** args, char* formato, char* flag, int flagLength);
int getModo(int nArgs, char** args, int* modo);
void imprimirSalida(FILE* fsalida, char formato, char* texto, int lon);
int prepararCBC(char* clave, char* bufferVecIni, int lenVecIni, int tamBloque, CBC* cbc);
void padding(char* texto, int* lon, int number, char padChar);
void imprimirSalida(FILE* fsalida, char formato, char* texto, int lon);
int leerEntrada(FILE* fentrada, char formato, char* texto, int maxLon);
void imprimirClave(FILE* fsalida, Bloque* clave);
void CFBmode(int modo, char* textoplano, char* textocifrado, int lon, CBC* cbc);
int esValida(Bloque* clave);
int hex2Bloque(Bloque* newbloque, char* string, int length);
void char2Hex(char* hex, uint8_t c);
void newClave(Bloque* clave);
int naleatorio(int a, int b);
void imprimirBloqueHexadecimal(FILE* fsalida, Bloque* b, int tamBloque, char* texto);
int comprobarHex(char hex);
void hex2Char(uint8_t* c, char* hex);
void texto2Bloque(Bloque* b, char* texto, int tamBloque);
void CFBstep(int flag, Bloque* bloqueTextoPlano, Bloque* bloqueTextoCifrado, CBC* cbc);
void bloque2Texto(char* texto, Bloque* b, int tamBloque);
void shiftRegister(Bloque* output, Bloque* input, int shift);
void char2Bin(uint8_t* bin, uint8_t c);
void bin2Hex(char* c, uint8_t* bin);
void hex2Bin(uint8_t* bin, char hex);
void DES(Bloque* output, Bloque* input, Bloque* clave, int flag);
void xorDES(Bloque* new, Bloque* old1, Bloque* old2, int length);
void bin2Char(uint8_t* c, uint8_t* bin);
void initialPerm(Bloque* new, Bloque* old);
void leftSemiBlock(Bloque* semiBlock, Bloque* fullBlock);
void rightSemiBlock(Bloque* semiBlock, Bloque* fullBlock);
void permChoice1(Bloque* new, Bloque* old);
void LCS(Bloque* new, Bloque* old, int nRound, int flag);
void shiftLeftDES(Bloque* new, Bloque* old, int shift);
void shiftRightDES(Bloque* new, Bloque* old, int shift);
void permChoice2(Bloque* new, Bloque* old);
void singleRound(Bloque* izqSalida, Bloque* derSalida, Bloque* izqEntrada, Bloque* derEntrada, Bloque* clave, int nRound);
void copiarBloque(Bloque* new, Bloque* old, int length);
void mergeSemiBlocks(Bloque* fullbloque, Bloque* left, Bloque* right);
void swap(Bloque* new, Bloque* old);
void invInitialPerm(Bloque* new, Bloque* old);
void selectDES(Bloque* new, Bloque* old, const unsigned short* indices, int length);
void expansion(Bloque* new, Bloque* old);
void SBox(Bloque* new, Bloque* old);
void permutation(Bloque* new, Bloque* old);
