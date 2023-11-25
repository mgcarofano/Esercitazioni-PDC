/*

	openmp.c
	di Mario Gabriele Carofano
	e Francesco Noviello

*/

/* **************************************************************************** */
// COSTANTI e CODICI D'ERRORE

#define NO_STRATEGY 0
#define NO_TEST 0

#define NO_TIME_CALC 0
#define OK_TIME_CALC 1

#define SUM_ONE_TEST 1
#define SUM_SINGLE_NUMBER_TEST 2
#define SUM_OPPOSITE_NUMBER_TEST 3
#define GAUSS_TEST 4
#define EXIT_TEST 5

#define OP_MAX_VALUE 100
#define OP_MAX_QUANTITY 20

#define TIME_PRECISION 1000000.0

#define NOT_IN_RANGE_ERROR 1
#define NOT_ENOUGH_OPERANDS 2
#define NOT_REAL_NUMBER_ERROR 3
#define INPUT_LINE_ERROR 4
#define NOT_NATURAL_NUMBER_ERROR 5

/* **************************************************************************** */
// LIBRERIE

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>
#include <tgmath.h>
#include <time.h>
#include <sys/time.h>

/* **************************************************************************** */
// FUNZIONI ACCESSORIE

double getNumberFromInput() {
	double out = 0;
	char *buffer = NULL;
	size_t bufsize = 0;
	ssize_t chars_read;

	// Inizializzazione del buffer con caratteri estratti dallo stream di input
	chars_read = getline(&buffer, &bufsize, stdin);
	printf("\n");

	if (chars_read < 0) {

		printf("Errore nella lettura dell'input!");
		printf("Applicazione terminata.\n");
		free(buffer);
		exit(INPUT_LINE_ERROR);

	} else {

		double digit_val = 0.0;
		int i = 0;
		int exp_whole = 0;
		int exp_fract = 0;
		int pos_point = -1;

		/*
			La lettura dei caratteri inizia dall'indice chars_read-2 perche':
			-	la numerazione del buffer è da 0 a N-1.
			-	l'ultimo carattere è \0.
		*/

		for (i = chars_read-2; i >= 0; i--) {
			if (buffer[i] != '.' && buffer[i] != ',') {
				exp_fract--;
			} else {
				pos_point = chars_read - (exp_fract * (-1)) -2;
				break;
			}
		}

		i = chars_read-2;
		while(i >= 0) {
			if (i > pos_point && pos_point != -1) {

				if (isdigit(buffer[i])) {
					// Per ottenere il valore intero a partire dal carattere ASCII
					digit_val = pow(10, exp_fract) * (digit_val + (buffer[i] - '0'));
					out = out + digit_val;
				} else {
					out = 0.0;
					printf("Puoi inserire solo valori numerici reali!\n");
					printf("Applicazione terminata.\n");
					free(buffer);
					exit(NOT_REAL_NUMBER_ERROR);
				}

				exp_fract++;

			} else if (i < pos_point || pos_point == -1) {

				if (isdigit(buffer[i])) {
					digit_val = pow(10, exp_whole) * (digit_val + (buffer[i] - '0'));
					out = out + digit_val;
				} else {
					if (i == 0 && buffer[0] == '-') {
						out = out * (-1);
					} else {
						out = 0.0;
						printf("Puoi inserire solo valori numerici reali!\n");
						printf("Applicazione terminata.\n");
						free(buffer);
						exit(NOT_REAL_NUMBER_ERROR);
					}
				}

				exp_whole++;
			}

			digit_val = 0.0;
			i--;
		}
	}

	free(buffer);
	return out;
}

int getIntegerFromInput() {

	double out_double = 0.0;
	int out_integer = 0;
	
	out_double = getNumberFromInput();
	out_integer = (int)out_double;

	if (out_integer < 0) {
		printf("Puoi inserire solo valori numerici naturali!\n");
		printf("Applicazione terminata.\n");
		exit(NOT_NATURAL_NUMBER_ERROR);
	}

	return out_integer;
}

void checkScelta(int scelta, int lim_inf, int lim_sup) {
	if(!(scelta >= lim_inf && scelta <= lim_sup)) {
		printf("Puoi inserire solo un valore numerico intero compreso tra %d e %d!\n", lim_inf, lim_sup);
		printf("Applicazione terminata.\n");
		exit(NOT_IN_RANGE_ERROR);
	}
}

/* **************************************************************************** */

int main(int argc, char **argv) {

	/* ************************************************************************ */
	// INIZIALIZZAZIONE DELL'AMBIENTE DI LAVORO

	int strategia = NO_STRATEGY, q_num = 0;
	int test = NO_TEST, time_calc = OK_TIME_CALC;
    int scelta = 0, i = 0, j = 0;

	int int_op = 0, int_rand = 0, gauss_inf = 0, gauss_tmp = 0;
	double *op, double_op = 0.0, double_rand = 0.0;
	
    double sum = 0.0;

    struct timeval t;
	double t_start = 0.0, t_end = 0.0, t_tot = 0.0;

    srand(time(NULL));

	/* ************************************************************************ */
	// INTRODUZIONE

	printf("\n");
	printf("Benvenuto nell'applicazione di testing per le esercitazioni di\n");
	printf("Parallel and Distributed Computing A.A. 2023-2024\n\n");

	printf("Questo programma consente di calcolare una somma ");
	printf("in ambiente MIMD a memoria condivisa.\n\n");

	/* ************************************************************************ */
	// SCELTA DELLA STRATEGIA DA APPLICARE

	do {
		/* **************************************************************** */
		// SELEZIONE DEL CASO DI TEST

		printf("Scegli un test da eseguire: \n");
		printf("%d. \t Somma di 1.\n", SUM_ONE_TEST);
		printf("%d. \t Somma di un singolo numero.\n", SUM_SINGLE_NUMBER_TEST);
		printf("%d. \t Somma di numeri interi opposti.\n", SUM_OPPOSITE_NUMBER_TEST);
		printf("%d. \t Somma di 'N' numeri naturali di un intervallo.\n", GAUSS_TEST);
		printf("%d. \t Chiudere la suite di testing.\n\n", EXIT_TEST);
		test = getIntegerFromInput();
		checkScelta(test, SUM_ONE_TEST, EXIT_TEST);

		if (test != EXIT_TEST) {

			printf("Inserisci la quantita' di numeri da sommare: \n");
			q_num = getIntegerFromInput();

			if (q_num <= 1) {
				printf("Devi inserire almeno due operandi!\n");
				printf("Applicazione terminata.\n");
				exit(NOT_ENOUGH_OPERANDS);
			}

			/* ************************************************************ */
			// GENERAZIONE DEGLI OPERANDI DA SOMMARE

			op = (double *)calloc (q_num, sizeof(double));

			switch(test) {
				case NO_TEST: {
					if (q_num <= OP_MAX_QUANTITY) {
						for (i=0; i < q_num; i++) {
							printf("Inserisci il %do operando da sommare: \n", i+1);
							double_op = getNumberFromInput();
							op[i] = double_op;
						}
					} else {
						for (i=0; i < q_num; i++) {
							double_rand = (double)rand();
							int_rand = (int)rand();

							// Si genera un numero casuale reale compreso tra 0 e 100
							op[j] = (double_rand / RAND_MAX) * OP_MAX_VALUE;

							// Si ha il 33% di possibilita che op[j] < 0
							if (int_rand % 3 == 0) {
								op[j] = op[j] * (-1);
							}
						}
					}
					break;
				}
				case SUM_ONE_TEST: {
					for (j=0; j < q_num; j++) {
						op[j] = 1;
					}
					break;
				}
				case SUM_SINGLE_NUMBER_TEST: {
					printf("Inserisci il valore dell'operando da sommare:\n");
					int_op = getIntegerFromInput();

					for (j=0; j < q_num; j++) {
						op[j] = int_op;
					}

					break;
				}
				case SUM_OPPOSITE_NUMBER_TEST: {
					for (j=0; j < q_num/2; j++) {

						double_rand = (double)rand();
						double_rand = (double_rand / RAND_MAX) * OP_MAX_VALUE;

						op[j] = double_rand;
						op[j+q_num/2] = double_rand * (-1);

					}
					break;
				}
				case GAUSS_TEST: {
					printf("Inserisci il limite inferiore dell'intervallo:\n");
					gauss_inf = getIntegerFromInput();

					for (j=0; j < q_num; j++) {
						op[j] = gauss_inf + gauss_tmp;
						gauss_tmp++;
					}

					gauss_tmp = 0;
					break;
				}
				default:
					break;
			}

			/* ************************************************************ */
			// INIZIO DEL CALCOLO DEI TEMPI DI ESECUZIONE

			if (time_calc == OK_TIME_CALC) {
				gettimeofday(&t, NULL);
				t_start = t.tv_sec + (t.tv_usec / TIME_PRECISION);
			}

			/* ************************************************************ */
			// CALCOLO DELLA SOMMA

			#pragma omp parallel for default(none) private(i) shared(q_num, op) reduction(+:sum)
			{
				for(i=0; i < q_num; i++) {
					sum = sum + op[i];
				}
			}

			/* ************************************************************ */
			// SALVATAGGIO DEL CALCOLO DEI TEMPI DI ESECUZIONE

			if (time_calc == OK_TIME_CALC) {
				gettimeofday(&t, NULL);
				t_end = t.tv_sec + (t.tv_usec / TIME_PRECISION);

				// Si calcola la distanza di tempo tra l'istante iniziale e quello finale.
				t_tot = t_end - t_start;

				printf("\nCalcolo della somma terminato in %e sec\n", t_tot);
			}

			/* ************************************************************ */
			// STAMPA DELL'OUTPUT

			printf("\nLa somma totale e' %f\n", sum);

			sum = 0;
			free(op);

		}

	} while (test != EXIT_TEST);

	/* ************************************************************************ */
	// USCITA DALL'APPLICATIVO

	printf("Applicazione terminata.\n");
	return 0;
}

/* **************************************************************************** */
/* RIFERIMENTI

    https://www.openmp.org/spec-html/5.0/openmp.html

*/
