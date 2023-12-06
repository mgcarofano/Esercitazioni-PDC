/*

	prova2.c
	di Mario Gabriele Carofano
	e Francesco Noviello

*/

/*	************************************************************************ */
//	LIBRERIE

#include "./libraries/auxfunc.h"
#include <omp.h>

/*	************************************************************************ */

int main(int argc, char **argv) {

	/*	******************************************************************** */
	//	INIZIALIZZAZIONE DELL'AMBIENTE DI LAVORO

	int strategia = NO_STRATEGY, test = NO_TEST, time_calc = NO_TIME_CALC;
	int rows = 0, cols = 0, threads = 0;
	int q_num = 0;

	int i = 0, j = 0, k = 0;
	double **mat, *vet;
	double *multiplication;

	struct timeval t;
	double t_start = 0.0, t_end = 0.0, t_tot = 0.0;

	int int_rand = 0;
	double double_rand = 0.0;

	srand(time(NULL));

	/*	******************************************************************** */
	//	LETTURA DEI DATI

	// I processori leggono gli argomenti in input 'argv[]'.
	if (argc < 5) {
		printf("Errore nella lettura degli argomenti di input!\n\n");
		printf("Esecuzione terminata.\n");
		exit(NOT_ENOUGH_ARGS_ERROR);
	}

	/*
		--- int argToInt(char *arg) ---
		Si utilizza la funzione 'argToInt' definita in 'auxfunc.h'
		per leggere gli argomenti contenuti nel vettore di 
		stringhe 'argv[]' e convertirli in valori interi.
	*/

	rows = argToInt(argv[1]);
	cols = argToInt(argv[2]);
	threads = argToInt(argv[3]);
	test = argToInt(argv[4]);
	time_calc = argToInt(argv[5]);

	mat = (double**) calloc(rows, sizeof(double*));
	for (i = 0; i < rows; i++) {
		mat[i] = (double*) calloc(cols, sizeof(double));
	}

	vet = (double*) calloc(cols, sizeof(double));

	multiplication = (double*) calloc(rows, sizeof(double));

	switch(test) {
		case NO_TEST:
		{
			q_num = rows * cols;
			k = 1;
			if (q_num <= OP_MAX_QUANTITY) {

				for (i = 0; i < rows; i++) {
					for (j = 0; j < cols; j++) {
						mat[i][j] = argToDouble(argv[k+5]);
						k++;
					}
				}

				for (j = 0; j < cols; j++) {
					vet[j] = argToDouble(argv[k+5]);
					k++;
				}

			} else {

				for (i = 0; i < rows; i++) {
					for (j = 0; j < cols; j++) {
						double_rand = (double)rand();
						int_rand = (int)rand();

						// Si genera un numero casuale reale compreso tra 0 e 100
						mat[i][j] = (double_rand / RAND_MAX) * OP_MAX_VALUE;

						// Si ha il 33% di possibilita che mat[i][j] < 0
						if (int_rand % 3 == 0) {
							mat[i][j] = mat[i][j] * (-1);
						}
					}
				}

				for (j = 0; j < cols; j++) {
					double_rand = (double)rand();
					int_rand = (int)rand();

					vet[j] = (double_rand / RAND_MAX) * OP_MAX_VALUE;

					if (int_rand % 3 == 0) {
						vet[j] = vet[j] * (-1);
					}
				}

			}

			break;
		}
		case MULTIPLICATION_ONE_TEST:
		{

			/*
				Il vettore da moltiplicare e' costituito di soli 1.

				Il test termina con successo se il vettore finale
				e' costituito dal valore della somma dei valori di
				ogni singola riga.
			*/

			for (i = 0; i < rows; i++) {
				for (j = 0; j < cols; j++) {
					double_rand = (double)rand();
					int_rand = (int)rand();

					mat[i][j] = (double_rand / RAND_MAX) * OP_MAX_VALUE;

					if (int_rand % 3 == 0) {
						mat[i][j] = mat[i][j] * (-1);
					}
				}
			}

			for (j = 0; j < cols; j++) {
				vet[j] = 1;
			}

			break;
		}
		case MULTIPLICATION_SINGLE_NUMBER_TEST:
		{

			/*
				Ad ogni posizione del vettore degli operandi e'
				assegnato lo stesso valore reale, passato come
				argomento 'argv[6]' al programma.

				Il test termina con successo se il vettore finale
				e' costituito dal valore della somma dei valori di
				ogni singola riga moltiplicato proprio per il
				valore 'argv[6]'.
			*/

			for (i = 0; i < rows; i++) {
				for (j = 0; j < cols; j++) {
					double_rand = (double)rand();
					int_rand = (int)rand();

					mat[i][j] = (double_rand / RAND_MAX) * OP_MAX_VALUE;

					if (int_rand % 3 == 0) {
						mat[i][j] = mat[i][j] * (-1);
					}
				}
			}

			for (j = 0; j < cols; j++) {
				vet[j] = argToDouble(argv[6]);
			}

			break;
		}
		case MULTIPLICATION_CSV_TEST:
		{

			/*
				In questo caso di test, i valori per la matrice ed il vettore
				sono caricati da un file .csv il cui percorso sono specificati
				alle posizioni 'argv[6]' e 'argv[7]'.
			*/

			// printf("rows: %d, cols: %d\n", rows, cols);

			getMatrixFromCSV(argv[6], mat, rows, cols);
			getVectorFromCSV(argv[7], vet, cols);
			break;
		}
		case MULTIPLICATION_EIGENVECTOR_TEST:
		{
			// Da implementare.
			// Si veda la sezione "Futuri sviluppi" nella documentazione.
			break;
		}
		default:
			break;
	}

	for (i = 0; i < rows; i++) {
		for (j = 0; j < cols; j++) {
			printf("Riga %d, ", i);
			printf("Colonna %d -> %f\n", j, mat[i][j]);
		}
	}

	for (j = 0; j < cols; j++) {
		printf("Colonna %d -> %f\n", j, vet[j]);
	}
	
	/*	******************************************************************** */
	//	DISTRIBUZIONE DEGLI OPERANDI

	/*
		Dato che questo programma e' progettato per essere eseguito su
		architettura MIMD a memoria condivisa, non si devono distribuire gli
		operandi della matrice e del vettore.
	*/

	/*	******************************************************************** */
	// 	INIZIO DEL CALCOLO DEI TEMPI DI ESECUZIONE

	if (time_calc == OK_TIME_CALC) {
		gettimeofday(&t, NULL);
		t_start = t.tv_sec + (t.tv_usec / TIME_PRECISION);
	}

	/*	******************************************************************** */
	//	APPLICAZIONE DELLA STRATEGIA

	/*
		Dato che non è stata eseguita la distribuzione degli operandi,
		anche l'applicazione della strategia di calcolo tra i processori,
		di conseguenza, si può omettere.
	*/

	/*	******************************************************************** */
	//	CALCOLO DEL PRODOTTO MATRICE-VETTORE

    omp_set_num_threads(threads);

	if (multiplication) {
        #pragma omp parallel for default(none) shared(rows, cols, mat, vet, multiplication) private (i,j)
        for (i = 0; i < rows; i++) {
            for (j = 0; j < cols; j++) {
                multiplication[i] = multiplication[i] + mat[i][j] * vet[j];
            }
        }
    }

	/*	******************************************************************** */
	//	SALVATAGGIO DEL CALCOLO DEI TEMPI DI ESECUZIONE

	if (time_calc == OK_TIME_CALC) {
		gettimeofday(&t, NULL);
		t_end = t.tv_sec + (t.tv_usec / TIME_PRECISION);

		// Si calcola la distanza di tempo tra l'istante iniziale e quello finale.
		t_tot = t_end - t_start;

		printf("\nCalcolo del prodotto matrice-vettore terminato in %e sec\n", t_tot);
		// writeTimeCSV(test, n_proc, rows, cols, t_tot);
	}

  	/*	******************************************************************** */
	//	STAMPA DELL'OUTPUT

	for (i = 0; i < rows; i++) {
        printf("Riga %d -> %f\n", i, multiplication[i]);
	}

  	/*	******************************************************************** */
	//	TERMINAZIONE DELL'ESECUZIONE

	/*
		--- void freeMatrix(double** mat, int rows) ---
		Si utilizza questa funzione per liberare la memoria allocata
		dalla matrice: non basta una sola free() per il puntatore double**,
		ma una free() per ogni puntatore double* ai vettori riga.
	*/

	freeMatrix(mat, rows);

	free(vet);
	free(multiplication);
	printf("\nEsecuzione terminata.\n");

	return 0;
}

/*	************************************************************************ */
/*	RIFERIMENTI

    https://www.openmp.org/spec-html/5.0/openmp.html
	https://www.studenti.it/matematica/prodotto-matrici-righe-colonne-4.jspc

*/
