#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
	
#define REPEATS 10
#define MAX_NOMBRE 300
#define TAM_ALFABETO 26 //Como el ingles

struct TABLA_PROBS {
	int m;//Tamanyo de alfabeto
	double *probabilidades;//Pp(x) o Pk(k)
	double **condicionadas;//Pp(x | y)
};


int getArgs(int nArgs, char** args, int* modo, char* ficheroentrada, int* entrada, char* ficherosalida, int* salida);
int getModo(int nArgs, char** args, int* modo);
int getCadena(int nArgs, char** args, char* cadena, char* modo, int longitud);
void iniProbsIguales(struct TABLA_PROBS* p, int m);
void iniProbsRandom(struct TABLA_PROBS* p, int m);
void iniTablaProbs(struct TABLA_PROBS* p, int m);
int obtainprobabilidades(struct TABLA_PROBS* resultado, FILE* fentrada, struct TABLA_PROBS* probs, int repeticiones);
void imprimirProbabilidades(FILE* f, struct TABLA_PROBS* p);
int getCharPlano(char c, int m);
int claveAleatoria(struct TABLA_PROBS* probs);
void freeprobabilidades(struct TABLA_PROBS* p);
