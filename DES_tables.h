#ifndef _DES_H_
#define _DES_H_

#define CIFRAR 1
#define DESCIFRAR 0
#define TAM_CLAVE 8

typedef struct {
    unsigned char k[TAM_CLAVE];
    unsigned char c[TAM_CLAVE/2];
    unsigned char d[TAM_CLAVE/2];
} subclave;

void generaClave(unsigned char* clave);
void generaSubClavesDES(unsigned char* clave, subclave* subclaves);
void DES(unsigned char* in, unsigned char* out, subclave* subclaves, int modo);
unsigned char ** DES_Avalancha(unsigned char* in, unsigned char* out, subclave* subclaves, int modo);
void DES_CBC(unsigned char* in, unsigned char* out, subclave* subclaves,unsigned char * chain, int modo);

#endif