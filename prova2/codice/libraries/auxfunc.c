/*

	auxfunc.c
	di Mario Gabriele Carofano
	e Francesco Noviello

*/

/* **************************************************************************** */
// LIBRERIE

#include "constants.c"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <tgmath.h>
#include <time.h>
#include <sys/time.h>

/* **************************************************************************** */
// CODICE DELLE FUNZIONI DEFINITE IN 'auxfunc.h'

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
		exit(INPUT_ARG_ERROR);
	}

	/*
		Per una corretta conversione da long a int, si controlla
		che il valore long sia rappresentabile come intero.
	*/
	
	if (out_long < INT_MIN || out_long > INT_MAX) {
		printf("Errore nella lettura degli argomenti di input!\n\n");
		printf("Esecuzione terminata.\n");
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
		exit(EMPTY_ARG_ERROR);
	}

	out_double = strtod(arg, &p);

	if (*p != '\0' || errno != 0) {
		printf("Errore nella lettura degli argomenti di input!\n\n");
		printf("Esecuzione terminata.\n");
		exit(INPUT_ARG_ERROR);
	}
	
	return out_double;
}

void writeTimeCSV(int test, int strategia, int n_proc, int q_num, double t_tot) {

	FILE *csv_file;

	system(MKDIR_PATH" -p "CSV_TIME_PATH);

	if ((csv_file = fopen(CSV_TIME_PATH"/"NOME_PROVA"_time.csv", "a")) == NULL) {
		printf("Errore durante l'esecuzione!\n");
		printf("Applicazione terminata.\n");
		exit(FILE_OPENING_ERROR);
	}

	fprintf(csv_file, "%d,%d,%d,%d,%1.9f\n",
		test, strategia, n_proc, q_num, t_tot);

	fclose(csv_file);

	printf("%s aggiornato con successo!\n\n", CSV_TIME_PATH"/"NOME_PROVA"_time.csv");

}

/* **************************************************************************** */
/* RIFERIMENTI



*/
