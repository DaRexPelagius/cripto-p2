
#include "NL_AES.h"



int DIRECT_AUX[BITS_IN_BYTE][BITS_IN_BYTE] = {
	{1, 0, 0, 0, 1, 1, 1, 1},
	{1, 1, 0, 0, 0, 1, 1, 1},
	{1, 1, 1, 0, 0, 0, 1, 1},
	{1, 1, 1, 1, 0, 0, 0, 1},
	{1, 1, 1, 1, 1, 0, 0, 0},
	{0, 1, 1, 1, 1, 1, 0, 0},
	{0, 0, 1, 1, 1, 1, 1, 0},
	{0, 0, 0, 1, 1, 1, 1, 1}
};

int* DIRECT_MATRIX[BITS_IN_BYTE] = {
	DIRECT_AUX[0], DIRECT_AUX[1], DIRECT_AUX[2], DIRECT_AUX[3],
	DIRECT_AUX[4], DIRECT_AUX[5], DIRECT_AUX[6], DIRECT_AUX[7]
};

int DIRECT_TRANSLATION[BITS_IN_BYTE] = {
	1, 1, 0, 0, 0, 1, 1, 0
};

int INVERSE_AUX[BITS_IN_BYTE][BITS_IN_BYTE] = {
	{0, 0, 1, 0, 0, 1, 0, 1},
	{1, 0, 0, 1, 0, 0, 1, 0},
	{0, 1, 0, 0, 1, 0, 0, 1},
	{1, 0, 1, 0, 0, 1, 0, 0},
	{0, 1, 0, 1, 0, 0, 1, 0},
	{0, 0, 1, 0, 1, 0, 0, 1},
	{1, 0, 0, 1, 0, 1, 0, 0},
	{0, 1, 0, 0, 1, 0, 1, 0}
};

int* INVERSE_MATRIX[BITS_IN_BYTE] = {
	INVERSE_AUX[0], INVERSE_AUX[1], INVERSE_AUX[2], INVERSE_AUX[3],
	INVERSE_AUX[4], INVERSE_AUX[5], INVERSE_AUX[6], INVERSE_AUX[7]
};

int INVERSE_TRANSLATION[BITS_IN_BYTE] = {
	1, 0, 1, 0, 0, 0, 0, 0
};

int IDENTITY[BITS_IN_BYTE+1] = {
	1, 1, 0, 1, 1, 0, 0, 0, 1
};

int main(int argc, char** argv) {

	int nTests,flagOutput;
	char outputFileName[MAX_NAME_LENGTH];
	FILE* outputFile = NULL;
	NL statistics;

	/* Gets command-line arguments */
	switch (getArgs(argc,argv,&nTests,outputFileName,&flagOutput)) {
		case ERR_N_ARGS:
			printUsage("ERROR: Wrong number of command-line arguments\n");
			return -1;
		case ERR_FLAGS:
			printUsage("ERROR: Wrong flags\n");
			return -1;
	}

	/* Validates command-line arguments */
	if (nTests < 1) {
		fprintf(stderr,"ERROR: The number of tests must be greater than 0\n");
		return -1;
	}

	if (flagOutput) {
		if ((outputFile = fopen(outputFileName,"w")) == NULL) {
			fprintf(stderr,"ERROR: Unable to open %s\n",outputFileName);
			return -1;
		}
	}
	else
		outputFile = stdout;

	/* AE measurement */
	measureNL(&statistics,nTests);
	printNL(outputFile,&statistics);

	if (outputFile && (outputFile != stdout))
		fclose(outputFile);

	return 0;
}

void printUsage(char* message) {

	if (message)
		fprintf(stderr,"%s",message);

	fprintf(stderr,"Syntax: ./desNL {-t nTests} [-o outputFile]\n");
	fprintf(stderr,"- nTests:\tNumber of tests to perform in order to measure the Avalanche Effect\n");
	fprintf(stderr,"- outputFile:\tOutput file (default: stdout)\n");
}


int getArgs(int nArgs, char** args, int* nTests, char* outputFileName, int* flagOutput) {

	if ((nArgs != 3) && (nArgs != 5))
		return ERR_N_ARGS;

	if (getInteger(nArgs,args,nTests,"-t",2) != 1)
		return ERR_FLAGS;

	if (nArgs == 3)
		*flagOutput = 0;
	else if (((*flagOutput) = getString(nArgs,args,outputFileName,"-o",2)) != 1)
		return ERR_FLAGS;

	return 0;
}


int getInteger(int nArgs, char** args, int* integer, char* flag, int flagLength) {

	int i;
	int found = 0;

	for (i=1; i<=nArgs-2; i++)
		if ((strncmp(args[i],flag,flagLength) == 0) && (strlen(args[i]) == flagLength)) {
			if (found)
				return -1;
			else {
				*integer = atoi(args[i+1]);
				if ((*integer == 0) && !isdigit(args[i+1][0]))
					return -1;
				found = 1;
			}
		}

	return found;
}

int getString(int nArgs, char** args, char* string, char* flag, int flagLength) {

	int i;
	int found = 0;

	for (i=1; i<=nArgs-2; i++)
		if ((strncmp(args[i],flag,flagLength) == 0) && (strlen(args[i]) == flagLength)) {
			if (found)
				return -1;
			else {
				strcpy(string,args[i+1]);
				found = 1;
			}
		}

	return found;
}

void measureNL(NL* statistics, int nTests) {

	unsigned long int coincCounters[SBOX_OUTPUT_BITS+1];
	float auxLinearity[SBOX_OUTPUT_BITS+1];
	struct AESBOX box;
	int i;
	float temp;

	srand(time(NULL));

	/* Initialises the counters */
	for (i=0; i < SBOX_OUTPUT_BITS+1; i++)
		coincCounters[i] = 0;

	/* Obtains the linearity distributions */
	calculateDirectBox(&box);
	for (i=0; i < nTests; i++)
		coincCounters[singleNL(&box)]++;

	/* Computes the statistics */
	for (i=0; i < SBOX_OUTPUT_BITS+1; i++)
		auxLinearity[i] = i*(1./SBOX_OUTPUT_BITS);


	/* Distribution */
	for (i=0; i < SBOX_OUTPUT_BITS+1; i++)
		statistics->distribution[i] = coincCounters[i]*(1./nTests);

	/* Expectation */
	statistics->expectation = 0;
	for (i=0; i < SBOX_OUTPUT_BITS+1; i++)
		statistics->expectation += (auxLinearity[i]*statistics->distribution[i]);

	/* Standard deviation */
	statistics->deviation = 0;
	for (i=0; i < SBOX_OUTPUT_BITS+1; i++) {
		temp = (auxLinearity[i] - statistics->expectation);
		statistics->deviation += temp*temp*statistics->distribution[i];
	}
	statistics->deviation = sqrt(statistics->deviation);
}

void printNL(FILE* outputFile, NL* statistics) {

	int j;

	fprintf(outputFile,"- Expectation:\t%.2f\n",statistics->expectation);
	fprintf(outputFile,"- Standard deviation:\t%.2f\n",statistics->deviation);
	fprintf(outputFile,"- Distribution of bit changes:\n");
	for (j=0; j < SBOX_OUTPUT_BITS+1; j++)
		fprintf(outputFile,"  - %d coincidences:\t%.2f%%\n",j, 100*statistics->distribution[j]);
}

void calculateDirectBox(struct AESBOX* box) {
	int i;
	for (i=0; i < BOX_SIZE; ++i) box->map[i] = codeDirect(i);
}

uint8_t codeDirect(uint8_t b) {
	return affineDirect(inverse(b));
}

uint8_t affineDirect(uint8_t b) {
	return affineTransformation(b, DIRECT_MATRIX, DIRECT_TRANSLATION);
}

uint8_t affineTransformation(uint8_t b, int *matrix[], int *translation) {
	int bits[BITS_IN_BYTE];
	int bits2[BITS_IN_BYTE];
	int i, j;
	uint8_t ret;
	
	/* obtain bitwise expansion of the byte */
	for (i=0; i < BITS_IN_BYTE; ++i) bits[i] = (b >> i)%2;
	
	/* multiplication by the matrix mod 2 */
	for (i=0; i < BITS_IN_BYTE; ++i) {
		bits2[i] = 0;
		for (j=0; j < BITS_IN_BYTE; ++j) bits2[i] += matrix[i][j]*bits[j];
		bits2[i] = bits2[i]%2;
	}
	
	/* addition of the vector mod 2 */
	for (i=0; i < BITS_IN_BYTE; ++i) bits2[i] = (bits2[i]+translation[i])%2;
	
	/* obtain the result as a byte from the array */
	ret = 0;
	for (i=0; i < BITS_IN_BYTE; ++i) ret += bits2[i] << i;
	
	return ret;
}

uint8_t inverse(uint8_t b) {
	struct POLYNOMIAL mod, p, inv;
	uint8_t ret;
	
	if (!b) return 0;
	
	initPol(&mod);
	initPol(&p);
	initPol(&inv);
	
	setPol(&mod, IDENTITY, BITS_IN_BYTE);
	byteToPoly(&p, b);
	
	inversePol(&inv, &p, &mod);
	ret = polyToByte(&inv);
	
	freePol(&mod);
	freePol(&p);
	freePol(&inv);
	
	return ret;
}

int singleNL(struct AESBOX* box) {

	uint8_t in1, in2, out1, out2;
	int k, counter, temp;

	in1 = rand()%256;
	out1 = box->map[in1];
	
	in2 = rand()%256;
	out2 = box->map[in2];
	
	temp = out1 ^ out2;

	/* Changes each possible input bit */
	counter = 0;
	for (k=0; k < SBOX_INPUT_BITS; k++) {
		counter += (temp & (1 << k)) != 0;
	}
	
	return counter;
}

void initPol(struct POLYNOMIAL* p) {
	p->degree = -1;
	p->coefficients = NULL;
}

void setPol(struct POLYNOMIAL* p, int* coefs, int n) {
	int i;

	if (p->coefficients) free(p->coefficients);
	p->coefficients = malloc(sizeof(int)*(n+1));
	for (i=0; i <= n; ++i) p->coefficients[i] = coefs[i]%2;
	p->degree = n;
}

void byteToPoly(struct POLYNOMIAL *p, uint8_t b) {
	int i;
  
	free(p->coefficients);
	p->coefficients = malloc(sizeof(int)*BITS_IN_BYTE);
	p->degree = BITS_IN_BYTE-1;
	for (i=0; i < BITS_IN_BYTE; ++i) p->coefficients[i] = (b >> i)%2;
}

void inversePol(struct POLYNOMIAL* dest, struct POLYNOMIAL* s, struct POLYNOMIAL* mod) {
	struct POLYNOMIAL q, r, p1, p2, v1, v2;
	int d1, d2;
	
	initPol(&q);
	initPol(&r);
	initPol(&p1);
	initPol(&p2);
	initPol(&v1);
	initPol(&v2);
	
	d1 = getDegree(mod);
	d2 = getDegree(s);
	
	copyPol(&p1, mod);
	if (d2 >= d1) divPol(&q, &p2, s, mod);
	else copyPol(&p2, s);
	
	clearPol(&v1);
	clearPol(&v2);
	setNthCoefficient(&v2, 0);
	
	while (1) {
		divPol(&q, &r, &p1, &p2);
		copyPol(&p1, &p2);
		copyPol(&p2, &r);
		
		if (getDegree(&r) < 0) break;
		
		mulPol(&r, &q, &v2);
		addPol(&r, &v1, &r);
		copyPol(&v1, &v2);
		copyPol(&v2, &r);
	}
	
	copyPol(dest, &v2);
	
	freePol(&q);
	freePol(&r);
	freePol(&p1);
	freePol(&p2);
	freePol(&v1);
	freePol(&v2);
}

uint8_t polyToByte(struct POLYNOMIAL* p) {
	uint8_t b = 0;
	int i;
	
	for (i=0; i < BITS_IN_BYTE && i <= p->degree; ++i) b += (p->coefficients[i] << i);
	return b;
}

void freePol(struct POLYNOMIAL* p) {
	p->degree = -1;
	free(p->coefficients);
	p->coefficients = NULL;
}

int getDegree(struct POLYNOMIAL* p) {
	int i;
	if (p->degree < 0) return -1;
	for (i=p->degree; i >= 0 && !p->coefficients[i]; i--);
	return i;
}

void mulPol(struct POLYNOMIAL* dest, struct POLYNOMIAL* s1, struct POLYNOMIAL* s2) {
	int d1 = s1->degree;
	int d2 = s2->degree;
	int d = d1+d2;
	int *aux = NULL;
	int i,j;
	
	if (d1 < 0 || d2 < 0) {
		if (dest->coefficients) free(dest->coefficients);
		dest->coefficients = NULL;
		dest->degree = -1; 
		return;
	}
	
	aux = malloc(sizeof(int)*(d+1));
	for (i=0; i <= d; ++i) {
		aux[i] = 0;
		for (j= (d1 < i)? d1 : i; j >= 0 && (i-j <= d2); j--) aux[i] += s1->coefficients[j]*s2->coefficients[i-j];
		aux[i] = aux[i]%2;
	}
  
	if (dest->coefficients) free(dest->coefficients);
	dest->coefficients = aux;
	dest->degree = d;
}

void addPol(struct POLYNOMIAL* dest, struct POLYNOMIAL* s1, struct POLYNOMIAL* s2) {
	int* aux = NULL;
	int greater = s1->degree > s2->degree;
	int d = greater ? s1->degree : s2->degree;
	int d2 = greater ? s2->degree : s1->degree;
	int i;
	
	/* if both polynomials had degree -1 */
	if (d == -1) {
		if (dest->coefficients) free(dest->coefficients);
		dest->coefficients = NULL;
		dest->degree = -1;
		return;
	}
	
	aux = malloc(sizeof(int)*(d+1));
	for (i=0; i <= d2; ++i) aux[i] = (s1->coefficients[i] + s2->coefficients[i])%2;
	for (i=d2+1; i <= d; ++i) aux[i] = greater ? s1->coefficients[i] : s2->coefficients[i];
  
	if (dest->coefficients) free(dest->coefficients);
	dest->coefficients = aux;
	dest->degree = d;
}

void copyPol(struct POLYNOMIAL* dest, struct POLYNOMIAL* src) {
	int* aux = NULL;
	int d = src->degree;
	int i;

	aux = malloc(sizeof(int)*(d+1));
	for (i=0; i <= d; ++i) aux[i] = src->coefficients[i];

	if (dest->coefficients) free(dest->coefficients);
	dest->coefficients = aux;
	dest->degree = d;
}

void divPol(struct POLYNOMIAL* q, struct POLYNOMIAL* r, struct POLYNOMIAL* div, struct POLYNOMIAL* dvsr) {
	int d1, d2;
	struct POLYNOMIAL aux, qAux, rAux;
	
	initPol(&aux);
	initPol(&qAux);
	initPol(&rAux);
	
	copyPol(&rAux, div);
	clearPol(&qAux);

	for (d1 = getDegree(&rAux), d2 = getDegree(dvsr);  d1 >= d2; d1 = getDegree(&rAux), d2 = getDegree(dvsr)) {
		shiftPol(&aux, dvsr, d1-d2);
		addPol(&rAux, &rAux, &aux);
		setNthCoefficient(&qAux, d1-d2);
	}
	
	copyPol(r, &rAux);
	copyPol(q, &qAux);
	
	freePol(&aux);
	freePol(&qAux);
	freePol(&rAux);
}

void clearPol(struct POLYNOMIAL* p) {
	int i;
	for (i=0; i <= p->degree; ++i) p->coefficients[i] = 0;
}

void setNthCoefficient(struct POLYNOMIAL* dest, int n) {
	int d = dest->degree, i;
	
	if (n < 0) return;
	
	if (n <= dest->degree) {
		dest->coefficients[n] = 1;
		return;
	}
	
	dest->coefficients = realloc(dest->coefficients, sizeof(int)*(n+1));
	for (i=d+1; i <n; ++i) dest->coefficients[i] = 0;
	dest->coefficients[n] = 1;
	dest->degree = n;
}

void shiftPol(struct POLYNOMIAL* dest, struct POLYNOMIAL* s, int n) {
	int* aux = NULL;
	int d = s->degree;
	int i;
	
	if (n < 0) return;
	
	aux = malloc(sizeof(int)*(d+n+1));
	for (i=0; i < n; ++i) aux[i] = 0;
	for (i=0; i <= d; ++i) aux[i+n] = s->coefficients[i];
	
	if (dest->coefficients) free(dest->coefficients);
	dest->coefficients = aux;
	dest->degree = d + n;
}


