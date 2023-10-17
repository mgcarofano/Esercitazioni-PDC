/* **************************************************************************** */
// LIBRERIE

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <tgmath.h>

/* **************************************************************************** */
// CODICI DI ERRORE

#define NOT_ENOUGH_ARGS_ERROR 1
#define EMPTY_ARG_ERROR 2
#define INPUT_ARG_ERROR 3
#define NOT_INT_ARG_ERROR 4
#define INPUT_LINE_ERROR 5
#define NOT_REAL_NUMBER_ERROR 6
#define NOT_NATURAL_NUMBER_ERROR 7
#define NOT_IN_RANGE_ERROR 8
#define NOT_ENOUGH_OPERANDS 9
#define FILE_OPENING_ERROR 10

/* **************************************************************************** */
// CODICE DELLE FUNZIONI DEFINITE IN 'auxfunc.h'

int argToInt(char *arg) {
	
	char *p;
	long out_long = 0;

	// Si inizializza il valore di 'errno' nella libreria <errno.h>
	errno = 0;

	/*
		Si preferisce l'utilizzo della funzione 'strtol' invece della
		funzione 'atoi' perche' consente un maggior numero di controlli
	*/

	out_long = strtol(arg, &p, 10);

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
	out_double = strtod(arg, &p);

	if (strlen(arg) == 0) {
		printf("Errore nella lettura degli argomenti di input!\n\n");
		printf("Esecuzione terminata.\n");
		MPI_Finalize();
		exit(EMPTY_ARG_ERROR);
	}

	if (*p != '\0' || errno != 0) {
		printf("Errore nella lettura degli argomenti di input!\n\n");
		printf("Esecuzione terminata.\n");
		MPI_Finalize();
		exit(INPUT_ARG_ERROR);
	}
	
	return out_double;
}

/* **************************************************************************** */
/* RIFERIMENTI



*/