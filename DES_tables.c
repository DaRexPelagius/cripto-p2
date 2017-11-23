#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DES_tables.h"

/* Constantes para el DES */
#define BITS_IN_PC1 56
#define BITS_IN_PC2 48
#define ROUNDS 16
#define BITS_IN_IP 64
#define BITS_IN_E 48
#define BITS_IN_P 32
#define NUM_S_BOXES 8
#define ROWS_PER_SBOX 4
#define COLUMNS_PER_SBOX 16


/* "permutacion" PC1 */
static const unsigned short PC1[BITS_IN_PC1] = { 
	57, 49, 41, 33, 25, 17, 9,
	1, 58, 50, 42, 34, 26, 18,
	10, 2, 59, 51, 43, 35, 27,
	19, 11, 3, 60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15,
	7, 62, 54, 46, 38, 30, 22,
	14, 6, 61, 53, 45, 37, 29,
	21, 13, 5, 28, 20, 12, 4
};

/* "permutacion" PC2 */
static const unsigned short PC2[BITS_IN_PC2] = {
	14, 17, 11, 24, 1, 5,
	3, 28, 15, 6, 21, 10,
	23, 19, 12, 4, 26, 8,
	16, 7, 27, 20, 13, 2,
	41, 52, 31, 37, 47, 55,
	30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53,
	46, 42, 50, 36, 29, 32
};

/* numero de bits que hay que rotar cada semiclave segun el numero de ronda */
static const unsigned short ROUND_SHIFTS[ROUNDS] = {
	1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

/* permutaciun IP */
static const unsigned short IP[BITS_IN_IP] = {
	58, 50, 42, 34, 26, 18, 10, 2,
	60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14, 6,
	64, 56, 48, 40, 32, 24, 16, 8,
	57, 49, 41, 33, 25, 17, 9, 1,
	59, 51, 43, 35, 27, 19, 11, 3,
	61, 53, 45, 37, 29, 21, 13, 5,
	63, 55, 47, 39, 31, 23, 15, 7
};

/* inversa de IP */
static const unsigned short IP_INV[BITS_IN_IP] = {
	40, 8, 48, 16, 56, 24, 64, 32,
	39, 7, 47, 15, 55, 23, 63, 31,
	38, 6, 46, 14, 54, 22, 62, 30,
	37, 5, 45, 13, 53, 21, 61, 29,
	36, 4, 44, 12, 52, 20, 60, 28,
	35, 3, 43, 11, 51, 19, 59, 27,
	34, 2, 42, 10, 50, 18, 58, 26,
	33, 1, 41, 9, 49, 17, 57, 25
};

/* expansi�n E */
static const unsigned short E[BITS_IN_E] = {
	32, 1, 2, 3, 4, 5,
	4, 5, 6, 7, 8, 9,
	8, 9, 10, 11, 12, 13,
	12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21,
	20, 21, 22, 23, 24, 25,
	24, 25, 26, 27, 28, 29,
	28, 29, 30, 31, 32, 1
};

/* permutaciun P */
static const unsigned short P[BITS_IN_P] = {
	16, 7, 20, 21,
	29, 12, 28, 17,
	1, 15, 23, 26,
	5, 18, 31, 10,
	2, 8, 24, 14,
	32, 27, 3, 9,
	19, 13, 30, 6,
	22, 11, 4, 25
};

/* cajas S */
static const unsigned short S_BOXES[NUM_S_BOXES][ROWS_PER_SBOX][COLUMNS_PER_SBOX] = {
	{	{ 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7 },
		{ 0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8 },
		{ 4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0 },
		{ 15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13 } 	},
	{
		{ 15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10 },
		{ 3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5 },
		{ 0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15 },
		{ 13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9 }	},

	{	{ 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8 },
		{ 13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1 },
		{ 13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7 },
		{ 1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12 }	},

	{	{ 7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15 },
		{ 13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9 },
		{ 10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4 },
		{ 3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14 }	},
	{
		{ 2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9 },
		{ 14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6 },
		{ 4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14 },
		{ 11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3 }	},
	{	
		{ 12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11 },
		{ 10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8 },
		{ 9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6 },
		{ 4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13 }	},
	{
 		{ 4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1 },
		{ 13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6 },
		{ 1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2 },
		{ 6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12 }	},
	{
		{ 13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7 },
		{ 1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2 },
		{ 7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8 },
		{ 2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11 }	}
};




void generaClave(unsigned char* clave) {
    int i;
    for (i=0; i<TAM_CLAVE; i++) {
        clave[i] = rand()%255;
    }
}


void generaSubClavesDES(unsigned char* clave, subclave* subclaves) {
    int i, j;
    int desp;
    unsigned char byte_desp, bits_desp1, bits_desp2, bits_desp3, bits_desp4;

    //Inicializamos a ceros
    for (i=0; i<8; i++) {
        subclaves[0].k[i] = 0;
    }
    //PC1
    for (i=0; i<BITS_IN_PC1; i++) {
        desp = PC1[i];
        //Mascara para tomar el bit de la posicion correcta de nuestra clave
        byte_desp = 0x80 >> ((desp - 1)%8);
        //Tomamos el byte correspondiente y cogemos el bit de nuestra clave con una AND
        byte_desp &= clave[(desp - 1)/8];
        //Si era un 1 lo movemos al principio si era 0 se queda como estaba
        byte_desp <<= ((desp - 1)%8);
        //Metemos en nuestra subclave el bit obtenido en la posicion que le corresponde moviendo en aritmetica modular 8 el bit
        subclaves[0].k[i/8] |= (byte_desp >> i%8);
    }

    for (i=0; i<3; i++) {
        subclaves[0].c[i] = subclaves[0].k[i];
    }
    //C llega hasta el bit 28 de los 56 es decir llega hasta la primera mitad del cuarto byte de K
    subclaves[0].c[3] = subclaves[0].k[3] & 0xF0;

    //Lo mismo para D pero empezando desde donde habiamos parado con c
    for (i=0; i<3; i++) {
        subclaves[0].d[i] = (subclaves[0].k[i+3] & 0x0F) << 4;
        subclaves[0].d[i] |= (subclaves[0].k[i+4] & 0xF0) >> 4;
    }
    //Para los ultimos 4 bits de D tomo solo los ultimos 4 de la clave
    subclaves[0].d[3] = (subclaves[0].k[6] & 0x0F) << 4;


    for (i=1; i<ROUNDS+1; i++) {
        //Rellenamos con la subclave anterior
        for (j=0; j<4; j++) {
            subclaves[i].c[j] = subclaves[i-1].c[j];
            subclaves[i].d[j] = subclaves[i-1].d[j];
        }

        desp = ROUND_SHIFTS[i];
        if (desp == 1){
            byte_desp = 0x80;
        } else {
            byte_desp = 0xC0;
        }

        // Aqui tomo los bits que voy a desplazar en C
        bits_desp1 = byte_desp & subclaves[i].c[0];
        bits_desp2 = byte_desp & subclaves[i].c[1];
        bits_desp3 = byte_desp & subclaves[i].c[2];
        bits_desp4 = byte_desp & subclaves[i].c[3];

        //Al primero le desplazo lo necesario a la izquierda y en los huecos que ha dejado meto los desplazados del anterior
        subclaves[i].c[0] <<= desp;
        subclaves[i].c[0] |= (bits_desp2 >> (8 - desp));
        //Lo mismo con el segundo
        subclaves[i].c[1] <<= desp;
        subclaves[i].c[1] |= (bits_desp3 >> (8 - desp));
        //Lo mismo con el tercero
        subclaves[i].c[2] <<= desp;
        subclaves[i].c[2] |= (bits_desp4 >> (8 - desp));
        //Para el cuarto hay que tener en cuenta que es circular y que ademas solo valen los 4 primeros bits por eso tomo los 4 bits del primer desplazamiento en los huecos que he dejado al mover el ultimo
        subclaves[i].c[3] <<= desp;
        subclaves[i].c[3] |= (bits_desp1 >> (4 - desp));

        // Mismo proceso de desplazamientos para D
        bits_desp1 = byte_desp & subclaves[i].d[0];
        bits_desp2 = byte_desp & subclaves[i].d[1];
        bits_desp3 = byte_desp & subclaves[i].d[2];
        bits_desp4 = byte_desp & subclaves[i].d[3];

        subclaves[i].d[0] <<= desp;
        subclaves[i].d[0] |= (bits_desp2 >> (8 - desp));

        subclaves[i].d[1] <<= desp;
        subclaves[i].d[1] |= (bits_desp3 >> (8 - desp));

        subclaves[i].d[2] <<= desp;
        subclaves[i].d[2] |= (bits_desp4 >> (8 - desp));

        subclaves[i].d[3] <<= desp;
        subclaves[i].d[3] |= (bits_desp1 >> (4 - desp));

        for (j=0; j<BITS_IN_PC2; j++) {
            //Mismo procedimiento con PC1 que con PC2 salvo que ahora tomamos solo 48 bits de las subclaves
            desp = PC2[j];                
            if (desp <= BITS_IN_PC1/2) {//Si es menor o igual que 28 es que estoy en C
                byte_desp = 0x80 >> ((desp - 1)%8);
                byte_desp &= subclaves[i].c[(desp - 1)/8];
                byte_desp <<= ((desp - 1)%8);
            } else {//Si es mayor que 28 es que es un bit de D
                byte_desp = 0x80 >> ((desp - 29)%8);
                byte_desp &= subclaves[i].d[(desp - 29)/8];
                byte_desp <<= ((desp - 29)%8);
            }
            subclaves[i].k[j/8] |= (byte_desp >> j%8);
        }
    }
    return;
}

void DES(unsigned char* in, unsigned char* out, subclave* subclaves, int modo) {
    int i, k,orden_clave;
    int desp;
    unsigned char byte_desp, aux_ip[8],l[4], r[4],li[4], ri[4], er[6], sbox_er[4],fila,col;
    memset(aux_ip, 0, 8);
    memset(out, 0, 8);

    for (i=0; i<BITS_IN_IP; i++) {
        /**
         * Igual que al generar las subclaves primero tomamos el byte a desplazar y vemos si es un 0 o un 1. Luego lo ponemos al principio
         * en nuestra variable auxiliar byte_desp y lo guardamos en la posicion correspondiente del byte correspondiente del bucle
         */
        desp = IP[i];
        byte_desp = 0x80 >> ((desp - 1)%8);
        byte_desp &= in[(desp - 1)/8];
        byte_desp <<= ((desp - 1)%8);

        aux_ip[i/8] |= (byte_desp >> i%8);
    }

    //Una vez permutada la clave se divide en parte izquierda y parte derecha
    for (i=0; i<4; i++) {
        l[i] = aux_ip[i];
        r[i] = aux_ip[i+4];
    }

    for (k=1; k<=ROUNDS; k++) {
        //Li=Ri-1
        memcpy(li, r, 4);
        memset(er, 0, 6);

        for (i=0; i<BITS_IN_E; i++) {
                //Hacemos lo mismo con E que con IP
                desp = E[i];
                byte_desp = 0x80 >> ((desp - 1)%8);
                byte_desp &= r[(desp - 1)/8];
                byte_desp <<= ((desp - 1)%8);
                er[i/8] |= (byte_desp >> i%8);
        }

        if (modo == DESCIFRAR) {
                orden_clave = 17 - k;
        } else {
                orden_clave = k;
        }

        //F=E(Ri-1) XOR Ki
        for (i=0; i<6; i++) {
            er[i] ^= subclaves[orden_clave].k[i];
        }

        memset(sbox_er, 0, 4);


        /**
         * En el primer calculo tomamos los bits 6 y 1 y los movemos a la derecha para calcular la fila
         * Hacemos lo mismo con los bits 5,4,3 moviendolos 3 posiciones a la derecha
         * A partir de aqui vamos rellenando los 24 bits de la salida de las sboxes teniendo en cuenta por el que nos ibamos
         * a la hora de aplicar las mascaras para coger los bits de e y hacer los desplazamientos
         */

        // Byte 1
        fila = 0;

        fila |= ((er[0] & 0x80) >> 6);
        fila |= ((er[0] & 0x04) >> 2);

        col = 0;
        //
        col |= ((er[0] & 0x78) >> 3);

        sbox_er[0] |= ((unsigned char)S_BOXES[0][fila][col] << 4);

        fila = 0;
        fila |= (er[0] & 0x02);
        fila |= ((er[1] & 0x10) >> 4);

        col = 0;
        col |= ((er[0] & 0x01) << 3);
        col |= ((er[1] & 0xE0) >> 5);

        sbox_er[0] |= (unsigned char)S_BOXES[1][fila][col];

        // Byte 2
        fila = 0;
        fila |= ((er[1] & 0x08) >> 2);
        fila |= ((er[2] & 0x40) >> 6);

        col = 0;
        col |= ((er[1] & 0x07) << 1);
        col |= ((er[2] & 0x80) >> 7);

        sbox_er[1] |= ((unsigned char)S_BOXES[2][fila][col] << 4);

        fila = 0;
        fila |= ((er[2] & 0x20) >> 4);
        fila |= (er[2] & 0x01);

        col = 0;
        col |= ((er[2] & 0x1E) >> 1);

        sbox_er[1] |= (unsigned char)S_BOXES[3][fila][col];

        // Byte 3
        fila = 0;
        fila |= ((er[3] & 0x80) >> 6);
        fila |= ((er[3] & 0x04) >> 2);

        col = 0;
        col |= ((er[3] & 0x78) >> 3);

        sbox_er[2] |= ((unsigned char)S_BOXES[4][fila][col] << 4);

        fila = 0;
        fila |= (er[3] & 0x02);
        fila |= ((er[4] & 0x10) >> 4);

        col = 0;
        col |= ((er[3] & 0x01) << 3);
        col |= ((er[4] & 0xE0) >> 5);

        sbox_er[2] |= (unsigned char)S_BOXES[5][fila][col];

        // Byte 4
        fila = 0;
        fila |= ((er[4] & 0x08) >> 2);
        fila |= ((er[5] & 0x40) >> 6);

        col = 0;
        col |= ((er[4] & 0x07) << 1);
        col |= ((er[5] & 0x80) >> 7);

        sbox_er[3] |= ((unsigned char)S_BOXES[6][fila][col] << 4);

        fila = 0;
        fila |= ((er[5] & 0x20) >> 4);
        fila |= (er[5] & 0x01);

        col = 0;
        col |= ((er[5] & 0x1E) >> 1);

        sbox_er[3] |= (unsigned char)S_BOXES[7][fila][col];

        memset(ri, 0, 4);

        for (i=0; i<BITS_IN_P; i++) {
            desp = P[i];
            byte_desp = 0x80 >> ((desp - 1)%8);
            byte_desp &= sbox_er[(desp - 1)/8];
            byte_desp <<= ((desp - 1)%8);

            ri[i/8] |= (byte_desp >> i%8);
        }

        for (i=0; i<4; i++) {
            ri[i] ^= l[i];
        }

        for (i=0; i<4; i++) {
            l[i] = li[i];
            r[i] = ri[i];
        }
    }
    //Concatenamos invirtiendo el orden antes de aplicar IP_INV
    for (i=0; i<4; i++) {
        aux_ip[i] = r[i];
        aux_ip[4+i] = l[i];
    }

    for (i=0; i<BITS_IN_IP; i++) {
        desp = IP_INV[i];
        byte_desp = 0x80 >> ((desp - 1)%8);
        byte_desp &= aux_ip[(desp - 1)/8];
        byte_desp <<= ((desp - 1)%8);
        out[i/8] |= (byte_desp >> i%8);
    }
    return;
        
}


unsigned char ** DES_Avalancha(unsigned char* in, unsigned char* out, subclave* subclaves, int modo) {
    int i, k,orden_clave;
    int desp;
    unsigned char byte_desp, aux_ip[8],l[4], r[4],li[4], ri[4], er[6], sbox_er[4],fila,col;
    unsigned char * ronda;
    memset(aux_ip, 0, 8);
    memset(out, 0, 8);
    unsigned char ** rondas=malloc(16*sizeof(unsigned char *));

    for (i=0; i<BITS_IN_IP; i++) {
        /**
         * Igual que al generar las subclaves primero tomamos el byte a desplazar y vemos si es un 0 o un 1. Luego lo ponemos al principio
         * en nuestra variable auxiliar byte_desp y lo guardamos en la posicion correspondiente del byte correspondiente del bucle
         */
        
        desp = IP[i];
        byte_desp = 0x80 >> ((desp - 1)%8);
        byte_desp &= in[(desp - 1)/8];
        byte_desp <<= ((desp - 1)%8);

        aux_ip[i/8] |= (byte_desp >> i%8);
    }

    //Una vez permutada la clave se divide en parte izquierda y parte derecha
    for (i=0; i<4; i++) {
        l[i] = aux_ip[i];
        r[i] = aux_ip[i+4];
    }

    for (k=1; k<=ROUNDS; k++) {
        //Li=Ri-1
        memcpy(li, r, 4);
        memset(er, 0, 6);

        for (i=0; i<BITS_IN_E; i++) {
            //Hacemos lo mismo con E que con IP
            desp = E[i];
            byte_desp = 0x80 >> ((desp - 1)%8);
            byte_desp &= r[(desp - 1)/8];
            byte_desp <<= ((desp - 1)%8);
            er[i/8] |= (byte_desp >> i%8);
        }

        if (modo == DESCIFRAR) {
            orden_clave = 17 - k;
        } else {
            orden_clave = k;
        }

        //F=E(Ri-1) XOR Ki
        for (i=0; i<6; i++) {
            er[i] ^= subclaves[orden_clave].k[i];
        }

        memset(sbox_er, 0, 4);


        /**
         * En el primer calculo tomamos los bits 6 y 1 y los movemos a la derecha para calcular la fila
         * Hacemos lo mismo con los bits 5,4,3 moviendolos 3 posiciones a la derecha
         * A partir de aqui vamos rellenando los 24 bits de la salida de las sboxes teniendo en cuenta por el que nos ibamos
         * a la hora de aplicar las mascaras para coger los bits de e y hacer los desplazamientos
         */

        // Byte 1
        fila = 0;

        fila |= ((er[0] & 0x80) >> 6);
        fila |= ((er[0] & 0x04) >> 2);

        col = 0;
        //
        col |= ((er[0] & 0x78) >> 3);

        sbox_er[0] |= ((unsigned char)S_BOXES[0][fila][col] << 4);

        fila = 0;
        fila |= (er[0] & 0x02);
        fila |= ((er[1] & 0x10) >> 4);

        col = 0;
        col |= ((er[0] & 0x01) << 3);
        col |= ((er[1] & 0xE0) >> 5);

        sbox_er[0] |= (unsigned char)S_BOXES[1][fila][col];

        // Byte 2
        fila = 0;
        fila |= ((er[1] & 0x08) >> 2);
        fila |= ((er[2] & 0x40) >> 6);

        col = 0;
        col |= ((er[1] & 0x07) << 1);
        col |= ((er[2] & 0x80) >> 7);

        sbox_er[1] |= ((unsigned char)S_BOXES[2][fila][col] << 4);

        fila = 0;
        fila |= ((er[2] & 0x20) >> 4);
        fila |= (er[2] & 0x01);

        col = 0;
        col |= ((er[2] & 0x1E) >> 1);

        sbox_er[1] |= (unsigned char)S_BOXES[3][fila][col];

        // Byte 3
        fila = 0;
        fila |= ((er[3] & 0x80) >> 6);
        fila |= ((er[3] & 0x04) >> 2);

        col = 0;
        col |= ((er[3] & 0x78) >> 3);

        sbox_er[2] |= ((unsigned char)S_BOXES[4][fila][col] << 4);

        fila = 0;
        fila |= (er[3] & 0x02);
        fila |= ((er[4] & 0x10) >> 4);

        col = 0;
        col |= ((er[3] & 0x01) << 3);
        col |= ((er[4] & 0xE0) >> 5);

        sbox_er[2] |= (unsigned char)S_BOXES[5][fila][col];

        // Byte 4
        fila = 0;
        fila |= ((er[4] & 0x08) >> 2);
        fila |= ((er[5] & 0x40) >> 6);

        col = 0;
        col |= ((er[4] & 0x07) << 1);
        col |= ((er[5] & 0x80) >> 7);

        sbox_er[3] |= ((unsigned char)S_BOXES[6][fila][col] << 4);

        fila = 0;
        fila |= ((er[5] & 0x20) >> 4);
        fila |= (er[5] & 0x01);

        col = 0;
        col |= ((er[5] & 0x1E) >> 1);

        sbox_er[3] |= (unsigned char)S_BOXES[7][fila][col];

        memset(ri, 0, 4);

        for (i=0; i<BITS_IN_P; i++) {
            desp = P[i];
            byte_desp = 0x80 >> ((desp - 1)%8);
            byte_desp &= sbox_er[(desp - 1)/8];
            byte_desp <<= ((desp - 1)%8);

            ri[i/8] |= (byte_desp >> i%8);
        }

        for (i=0; i<4; i++) {
            ri[i] ^= l[i];
        }
        rondas[k-1]=(unsigned char *) malloc(8*sizeof(unsigned char)); 
        strncpy(rondas[k-1],l,4);
        strncpy(rondas[k-1]+4,r,4);
        
        
        for (i=0; i<4; i++) {
            l[i] = li[i];
            r[i] = ri[i];
        }
    }
    //Concatenamos invirtiendo el orden antes de aplicar IP_INV
    for (i=0; i<4; i++) {
        aux_ip[i] = r[i];
        aux_ip[4+i] = l[i];
    }

    for (i=0; i<BITS_IN_IP; i++) {
        desp = IP_INV[i];
        byte_desp = 0x80 >> ((desp - 1)%8);
        byte_desp &= aux_ip[(desp - 1)/8];
        byte_desp <<= ((desp - 1)%8);
        out[i/8] |= (byte_desp >> i%8);
    }
    return rondas;
        
}

void DES_CBC(unsigned char* in, unsigned char* out, subclave* subclaves,unsigned char * chain, int modo) {
    int i, k,orden_clave;
    int desp;
    unsigned char byte_desp, aux_ip[8],l[4], r[4],li[4], ri[4], er[6], sbox_er[4],fila,col,IV[8];
    memset(aux_ip, 0, 8);
    memset(out, 0, 8);
    generaClave(IV);

    if(modo==CIFRAR){
        for (i=0;i<TAM_CLAVE;i++){
           in[i]^=chain[i]; 
        }
    }
    for (i=0; i<BITS_IN_IP; i++) {
        /**
         * Igual que al generar las subclaves primero tomamos el byte a desplazar y vemos si es un 0 o un 1. Luego lo ponemos al principio
         * en nuestra variable auxiliar byte_desp y lo guardamos en la posicion correspondiente del byte correspondiente del bucle
         */
        desp = IP[i];
        byte_desp = 0x80 >> ((desp - 1)%8);
        byte_desp &= in[(desp - 1)/8];
        byte_desp <<= ((desp - 1)%8);

        aux_ip[i/8] |= (byte_desp >> i%8);
    }

    //Una vez permutada la clave se divide en parte izquierda y parte derecha
    for (i=0; i<4; i++) {
        l[i] = aux_ip[i];
        r[i] = aux_ip[i+4];
    }

    for (k=1; k<=ROUNDS; k++) {
        //Li=Ri-1
        memcpy(li, r, 4);
        memset(er, 0, 6);

        for (i=0; i<BITS_IN_E; i++) {
                //Hacemos lo mismo con E que con IP
                desp = E[i];
                byte_desp = 0x80 >> ((desp - 1)%8);
                byte_desp &= r[(desp - 1)/8];
                byte_desp <<= ((desp - 1)%8);
                er[i/8] |= (byte_desp >> i%8);
        }

        if (modo == DESCIFRAR) {
                orden_clave = 17 - k;
        } else {
                orden_clave = k;
        }

        //F=E(Ri-1) XOR Ki
        for (i=0; i<6; i++) {
            er[i] ^= subclaves[orden_clave].k[i];
        }

        memset(sbox_er, 0, 4);


        /**
         * En el primer calculo tomamos los bits 6 y 1 y los movemos a la derecha para calcular la fila
         * Hacemos lo mismo con los bits 5,4,3 moviendolos 3 posiciones a la derecha
         * A partir de aqui vamos rellenando los 24 bits de la salida de las sboxes teniendo en cuenta por el que nos ibamos
         * a la hora de aplicar las mascaras para coger los bits de e y hacer los desplazamientos
         */

        // Byte 1
        fila = 0;

        fila |= ((er[0] & 0x80) >> 6);
        fila |= ((er[0] & 0x04) >> 2);

        col = 0;
        //
        col |= ((er[0] & 0x78) >> 3);

        sbox_er[0] |= ((unsigned char)S_BOXES[0][fila][col] << 4);

        fila = 0;
        fila |= (er[0] & 0x02);
        fila |= ((er[1] & 0x10) >> 4);

        col = 0;
        col |= ((er[0] & 0x01) << 3);
        col |= ((er[1] & 0xE0) >> 5);

        sbox_er[0] |= (unsigned char)S_BOXES[1][fila][col];

        // Byte 2
        fila = 0;
        fila |= ((er[1] & 0x08) >> 2);
        fila |= ((er[2] & 0x40) >> 6);

        col = 0;
        col |= ((er[1] & 0x07) << 1);
        col |= ((er[2] & 0x80) >> 7);

        sbox_er[1] |= ((unsigned char)S_BOXES[2][fila][col] << 4);

        fila = 0;
        fila |= ((er[2] & 0x20) >> 4);
        fila |= (er[2] & 0x01);

        col = 0;
        col |= ((er[2] & 0x1E) >> 1);

        sbox_er[1] |= (unsigned char)S_BOXES[3][fila][col];

        // Byte 3
        fila = 0;
        fila |= ((er[3] & 0x80) >> 6);
        fila |= ((er[3] & 0x04) >> 2);

        col = 0;
        col |= ((er[3] & 0x78) >> 3);

        sbox_er[2] |= ((unsigned char)S_BOXES[4][fila][col] << 4);

        fila = 0;
        fila |= (er[3] & 0x02);
        fila |= ((er[4] & 0x10) >> 4);

        col = 0;
        col |= ((er[3] & 0x01) << 3);
        col |= ((er[4] & 0xE0) >> 5);

        sbox_er[2] |= (unsigned char)S_BOXES[5][fila][col];

        // Byte 4
        fila = 0;
        fila |= ((er[4] & 0x08) >> 2);
        fila |= ((er[5] & 0x40) >> 6);

        col = 0;
        col |= ((er[4] & 0x07) << 1);
        col |= ((er[5] & 0x80) >> 7);

        sbox_er[3] |= ((unsigned char)S_BOXES[6][fila][col] << 4);

        fila = 0;
        fila |= ((er[5] & 0x20) >> 4);
        fila |= (er[5] & 0x01);

        col = 0;
        col |= ((er[5] & 0x1E) >> 1);

        sbox_er[3] |= (unsigned char)S_BOXES[7][fila][col];

        memset(ri, 0, 4);

        for (i=0; i<BITS_IN_P; i++) {
            desp = P[i];
            byte_desp = 0x80 >> ((desp - 1)%8);
            byte_desp &= sbox_er[(desp - 1)/8];
            byte_desp <<= ((desp - 1)%8);

            ri[i/8] |= (byte_desp >> i%8);
        }

        for (i=0; i<4; i++) {
            ri[i] ^= l[i];
        }

        for (i=0; i<4; i++) {
            l[i] = li[i];
            r[i] = ri[i];
        }
    }
    //Concatenamos invirtiendo el orden antes de aplicar IP_INV
    for (i=0; i<4; i++) {
        aux_ip[i] = r[i];
        aux_ip[4+i] = l[i];
    }

    for (i=0; i<BITS_IN_IP; i++) {
        desp = IP_INV[i];
        byte_desp = 0x80 >> ((desp - 1)%8);
        byte_desp &= aux_ip[(desp - 1)/8];
        byte_desp <<= ((desp - 1)%8);
        out[i/8] |= (byte_desp >> i%8);
    }
    if(modo==DESCIFRAR){
        for (i=0;i<TAM_CLAVE;i++){
           out[i]^=chain[i]; 
        }
    }
    
    return;
        
}