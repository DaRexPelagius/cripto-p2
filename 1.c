#include "1.h"


int main(int argc, char** argv) {

	int modo;//Flag donde guardaremos si es equiprobable o no.(0 equiprobable, 1 no)
	int entrada, salida;
	char ficheroentrada[MAX_NOMBRE];
	char ficherosalida[MAX_NOMBRE];
	FILE* fentrada = NULL;
	FILE* fsalida = NULL;
	struct TABLA_PROBS probs;
	struct TABLA_PROBS resultado;
	
	/* random seed */
	srand(time(NULL));

	//Comprobamos que los argumentos son los correctos, 2 si no se indican ficheros de entrada y de salida
	//4 si solo se indica un fichero y 6 si se indican ambos
	if ((argc != 2) && (argc != 4) && (argc != 6)){
		printf("Número de argumentos incorrecto.\n");
		printf("Uso del programa: ./1 {-P | -I} [-i filein] [-o fileout].\n");
		return -1;
	}

	//Como en la practica anterior usamos una funcion auxiliar para conseguir los argumentos
	if (getArgs(argc, argv, &modo, ficheroentrada, &entrada, ficherosalida, &salida) == -1) {
		printf("Argumentos incorrectos.\n");
		printf("Uso del programa: ./1 {-P | -I} [-i filein] [-o fileout].\n");
		return -1;
	}
	//Si se indico un fichero de entrada lo abrimos
	if (entrada) {
		if ((fentrada = fopen(ficheroentrada, "r")) == NULL) {
			fprintf(stderr, "Error al abrir el fichero de entrada %s\n", ficheroentrada);
			return -1;
		}
	} else fentrada = stdin;
	//Si se indico un fichero de salida lo abrimos
	if (salida) {
		if ((fsalida = fopen(ficherosalida, "w")) == NULL) {
			fprintf(stderr, "Error al abrir el fichero de salida %s\n", ficherosalida);
			if (fentrada) fclose(fentrada);
			return -1;
		}
	} else fsalida = stdout;

	//Iniciamos las probabilidades de las claves
	//utilizamos una estructura como la de resultado por comodidad
	if (modo == 0) iniProbsIguales(&probs, TAM_ALFABETO);
	else iniProbsRandom(&probs, TAM_ALFABETO);
	
	//Reservamos una tabla para guardar los resultados
	iniTablaProbs(&resultado, TAM_ALFABETO);
	obtainprobabilidades(&resultado, fentrada, &probs, REPEATS);
	fprintf(stdout, "Las probabilidades del espacio de claves: \n");
	writeprobabilidades(stdout, &probs);
	fprintf(stdout, "\nLas probabilidades de encontrar el texto plano en el texto:\n");
	writeprobabilidades(fsalida, &resultado);
	
	
	/* memory clear */
	freeprobabilidades(&resultado);
	freeprobabilidades(&probs);
	if (fentrada) fclose(fentrada);
	if (fsalida) fclose(fsalida);

	return 0;
}

int getArgs(int nArgs, char** args, int* flag, char* ficheroentrada, int* entrada, char* ficherosalida, int* salida) {
	if (getModo(nArgs,args,flag) != 1)	return -1;

	if (nArgs == 2) {
		*entrada = 0;
		*salida = 0;
	}
	else {
		if (((*entrada) = getCadena(nArgs,args,ficheroentrada,"-i",2)) == -1) return -1;
		if (((*salida) = getCadena(nArgs,args,ficherosalida,"-o",2)) == -1) return -1;
		if ((nArgs == 4) && (*entrada + *salida != 1)) return -1;
		if ((nArgs == 6) && (*entrada + *salida != 2)) return -1;
	}
	return 0;
}


//Si se ejecuta normalmente devuelve 0, sino -1
int getModo(int nArgs, char** args, int* modo) {
	int i;
	int aux = -1;
	for (i = 1; i < nArgs; i++){
		if ((strncmp(args[i], "-P", 2) == 0) && (strlen(args[i]) == 2)) {
			*modo = 1; aux = 0; break;
		}
		else if ((strncmp(args[i],"-I",2) == 0) && (strlen(args[i]) == 2)) {
			*modo = 0; aux = 0; break;
		}
	}
	return aux;
}

/*--------------------------------------------------------------------------
Obtiene una cadena
- Entrada:
	* Número de argumentos
	* Array de argumentos
	* Puntero a la cadena
	* Puntero al modo
	* Longitud de la cadena
- Salida:
	* -1 si ocurre algun error
	* 1 si va bien
--------------------------------------------------------------------------*/
int getCadena(int nArgs, char** args, char* cadena, char* modo, int longitud) {

	int i;
	int flag = 0;

	for (i = 1; i <= (nArgs - 2); i++)
		if ((strncmp(args[i], modo, longitud) == 0) && (strlen(args[i]) == longitud)) {
			if (flag)
				return -1;
			else {
				strcpy(cadena, args[i+1]);
				flag = 1;
			}
		}

	return flag;
}
//Iniciamos las probabilidades de forma equiprobable,
//las guardamos en una tabla de propabilidades
void iniProbsIguales(struct TABLA_PROBS* p, int m) {
	int i;
	
	p->m = m;
	p->probabilidades = malloc(sizeof(double) * m);
	for (i = 0; i < m; i++) p->probabilidades[i] = 1.0 / m;
	
	p->condicionadas = NULL;
}
//Iniciamos las probabilidades de forma NO equiprobable,
//las guardamos en una tabla de propabilidades
void iniProbsRandom(struct TABLA_PROBS* p, int m) {
	int i;
	double total = 0;
	
	p->m = m;
	p->probabilidades = malloc(sizeof(double) * m);
	for (i=0; i < m; ++i) p->probabilidades[i] = (rand()+1.0) / RAND_MAX;
	for (i=0; i < m; ++i) total += p->probabilidades[i];
	for (i=0; i < m; ++i)  p->probabilidades[i] /= total;
	
	p->condicionadas = NULL;
}
//Reservamos memoria para el resultado
void iniTablaProbs(struct TABLA_PROBS* p, int m) {
	int i, j;
	
	p->m = m;//Pegamos el tamanyo de alfabeto
	p->probabilidades = malloc(sizeof(double) * m);
	p->condicionadas = malloc(sizeof(double*) * m);
	for (i = 0; i < m; i++) p->condicionadas[i] = malloc(sizeof(double) * m);
	for (i = 0; i < m; ++i) p->probabilidades[i] = 0;
	for (i = 0; i < m; ++i) for (j = 0; j < m; ++j) p->condicionadas[i][j] = 0;
}

int obtainprobabilidades(struct TABLA_PROBS* resultado, FILE* fentrada, struct TABLA_PROBS* probs, int repeats) {
	
	int i,j;
	int m = probs->m;//Tam alfabeto
	double count;
	double *frecuencyCipher = NULL;
	int key, plain, cipher;
	char c;
	struct TABLA_PROBS pCifrado;
	
	/* memory management */
	frecuencyCipher = malloc(sizeof(double) * m);
	pCifrado.probabilidades = malloc(sizeof(double) * m);
	pCifrado.m = m;
	
	/* clear to zeros and obtain pCifrado*/
	for (i=0; i < m; ++i) frecuencyCipher[i] = 0;
	pCifrado.probabilidades[0] = probs->probabilidades[0];
	for (i=1; i < m; ++i) pCifrado.probabilidades[i] = pCifrado.probabilidades[i-1] + probs->probabilidades[i];
	for (i=0; i < m; ++i) resultado->probabilidades[i] = 0;
	for (i=0; i < m; ++i) for (j=0; j < m; ++j) resultado->condicionadas[i][j] = 0;
	
	
	count = 0;
	for (i=0; i < repeats; ++i) {
		/* rewind file so that lecture starts from the beggining */
		rewind(fentrada);

		/* main loop */
		while (((c = fgetc(fentrada)) != EOF) && (fentrada != stdin || c != '\n')) {
			key = randomKey(&pCifrado);
			plain = getPlain(c, m);
			if (plain < 0 || plain >= m) continue;
			else count++;
			cipher = (plain+key)%m;
			
			resultado->probabilidades[plain]++;
			resultado->condicionadas[plain][cipher]++;
			frecuencyCipher[cipher]++;
		}
	}
	
	/* set results */
	for (i=0; i < m; ++i) resultado->probabilidades[i] /= count;
	for (i=0; i < m; ++i) for (j=0; j < m; ++j) resultado->condicionadas[i][j] = frecuencyCipher[j]? resultado->condicionadas[i][j]/frecuencyCipher[j]:0;
	
	/* free memory and return */
	free(frecuencyCipher);
	free(pCifrado.probabilidades);
	return 0;
}
//Escribe la estructura probabilidades
void writeprobabilidades(FILE* f, struct TABLA_PROBS* p) {
	int i,j;
	int m = p->m;
	
	for (i=0; i < m; i++) fprintf(f, "P(%c) = %lf\n", i+'A', p->probabilidades[i]);
	printf("\n");
	if (p->condicionadas) {
		for (i=0; i < m; i++) {
			for (j=0; j < m; j++) fprintf(f, "P(%c|%c) = %lf, ", i+'A', j+'A', p->condicionadas[i][j]);
			printf("\n\n");
		}
	}
}
//c = x mod m
int getPlain(char c, int m) {
	if ((c >= 'A') && (c <= 'A'+m-1)) return c - 'A';
	else if ((c >= 'a') && (c <= 'a'+m-1)) return c - 'a';
	else return -1;
}
//Obtiene una clave aleatoria de una distribucion de frecuencia	
int randomKey(struct TABLA_PROBS* probs) {
	double value;
	int i;
	
	value = (rand()+1.0)/RAND_MAX;
	for (i=0; i < probs->m; ++i) {
		if (value <= probs->probabilidades[i]) return i;
	}
	return probs->m - 1;
}
//Liberamos memoria
void freeprobabilidades(struct TABLA_PROBS* p) {
	int i;
	
	if (p->probabilidades) free(p->probabilidades);
	p->probabilidades = NULL;
	
	if (p->condicionadas) {
		for (i=0; i < p->m; i++) free(p->condicionadas[i]);
		free(p->condicionadas);
		p->condicionadas = NULL;
	}
	p->m = 0;
}