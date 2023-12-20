/*

	auxfunc.c
	di Mario Gabriele Carofano
	e Francesco Noviello

*/

/*	**************************************************************************** */
//	LIBRERIE

#include "constants.c"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

/*	**************************************************************************** */
//	CODICE DELLE FUNZIONI DEFINITE IN 'auxfunc.h'

int argToInt(char *arg) {
	
	char *p;
	long out_long = 0;

	// Si inizializza il valore di 'errno' nella libreria <errno.h>
	errno = 0;

	/*
		Si controlla che l'argomento passato al programma non sia una stringa
		vuota. La funzione 'strlen', infatti, ne misura la dimensione.
	*/

	if (strlen(arg) == 0) {
		printf("Errore nella lettura degli argomenti di input!\n\n");
		printf("Esecuzione terminata.\n");
		MPI_Finalize();
		exit(EMPTY_ARG_ERROR);
	}

	/*
		Si preferisce l'utilizzo della funzione 'strtol' invece della
		funzione 'atoi' perche' consente un maggior numero di controlli
	*/

	out_long = strtol(arg, &p, 10);

	/*
		Si controlla che l'argomento passato al programma non sia una stringa
		contenente il solo carattere '\0' (null-termination character) e non
		ci siano stati altri errori durante la sua lettura.
	*/

	if (*p != '\0' || errno != 0) {
		printf("Errore nella lettura degli argomenti di input!\n\n");
		printf("Esecuzione terminata.\n");
		MPI_Finalize();
		exit(INPUT_ARG_ERROR);
	}

	/*
		Per una corretta conversione da long a int, si controlla
		che il valore long sia rappresentabile come intero.
	*/
	
	if (out_long < INT_MIN || out_long > INT_MAX) {
		printf("Errore nella lettura degli argomenti di input!\n\n");
		printf("Esecuzione terminata.\n");
		MPI_Finalize();
		exit(NOT_INT_ARG_ERROR);
	}
	
	return (int)out_long;
}

double argToDouble(char *arg) {
	
	char *p;
	double out_double = 0.0;

	errno = 0;

	if (strlen(arg) == 0) {
		printf("Errore nella lettura degli argomenti di input!\n\n");
		printf("Esecuzione terminata.\n");
		MPI_Finalize();
		exit(EMPTY_ARG_ERROR);
	}

	out_double = strtod(arg, &p);

	if (*p != '\0' || errno != 0) {
		printf("Errore nella lettura degli argomenti di input!\n\n");
		printf("Esecuzione terminata.\n");
		MPI_Finalize();
		exit(INPUT_ARG_ERROR);
	}
	
	return out_double;
}

void getRandomMatrix(double *mat, int mat_rows, int mat_cols) {

	int i = 0, j = 0;
	double double_rand = 0.0;
	int int_rand = 0;

	for (i = 0; i < mat_rows; i++) {
		for (j = 0; j < mat_cols; j++) {
			double_rand = (double)rand();
			int_rand = (int)rand();

			// Si genera un numero casuale reale compreso tra 0 e OP_MAX_VALUE
			mat[i*mat_cols + j] = (double_rand / RAND_MAX) * OP_MAX_VALUE;

			// Si ha il 33% di possibilita che mat[i*mat_cols + j] < 0
			if (int_rand % 3 == 0) {
				mat[i*mat_cols + j] *= -1;
			}
		}
	}
	
}

void getIdentityMatrix(double *mat, int mat_rows, int mat_cols) {

	int i = 0, j = 0;

	for (i = 0; i < mat_rows; i++) {
		for (j = 0; j < mat_cols; j++) {
			if (i == j) {
				mat[i*mat_cols + j] = 1;
			}
		}
	}

}

void getTransposeMatrix(double *A_mat, int A_rows, int A_cols, double *B_mat, int B_rows, int B_cols) {

	int i = 0, j = 0;

	if (A_rows != B_cols || A_cols != B_rows) {
		printf("Errore nella costruzione della matrice trasposta!\n\n");
		printf("Esecuzione terminata!\n");
		MPI_Finalize();
		exit(MATRIX_DIMENSION_ERROR);
	}

	for (j = 0; j < A_cols; j++) {
		for (i = 0; i < A_rows; i++) {
			B_mat[j*B_cols + i] = A_mat[i*A_cols + j];
		}
	}

}

/*	***************************************************************************
	RIFERIMENTI
	
*/
