/*

	prova2.c
	di Mario Gabriele Carofano
	e Francesco Noviello

*/

/* **************************************************************************** */
// LIBRERIE

#include "./libraries/auxfunc.h"

/* **************************************************************************** */

int main(int argc, char **argv) {

	/* ************************************************************************ */
	// INIZIALIZZAZIONE DELL'AMBIENTE DI LAVORO

	int strategia = NO_STRATEGY, q_num = 0;
	int test = NO_TEST, time_calc = NO_TIME_CALC;

	int id_proc = 0;

	timeval time;
	double t_start = 0.0, t_end = 0.0, t_tot = 0.0;
	double double_rand = 0.0;

	srand(time(NULL));

	/* ************************************************************************ */
	// LETTURA DEI DATI
	
	/* ************************************************************************ */
	// DISTRIBUZIONE DEGLI OPERANDI -> ESSENDO MEMORIA CONDIVISA NON SI DEVE DISTRIBUIRE NULLA

	/* ************************************************************************ */
	// INIZIO DEL CALCOLO DEI TEMPI DI ESECUZIONE

	if (time_calc == OK_TIME_CALC) {
		gettimeofday(&time, NULL);
		t_start = time.tv_sec + (time.tv_usec / TIME_PRECISION);
	}

	/* ************************************************************************
	// E NON SI APPLICANO LE STRATEGIE
	// APPLICAZIONE DELLA STRATEGIA

	switch (strategia) {
		case NO_STRATEGY: {
			break;
		}
		case FIRST_STRATEGY: // Applicazione della strategia 1.
		{
			break;
		}
		case SECOND_STRATEGY: // Applicazione della strategia 2.
		{
			break;
		}
		case THIRD_STRATEGY: // Applicazione della strategia 3.
		{
			break;
		}
		default:
		{
			printf("Comando non riconosciuto!\n");
			break;
		}
	}
	*/

	/* ************************************************************************ */
	// SALVATAGGIO DEL CALCOLO DEI TEMPI DI ESECUZIONE

	if (time_calc == OK_TIME_CALC) {
		gettimeofday(&time, NULL);
		t_end = time.tv_sec + (time.tv_usec / TIME_PRECISION);

		// Si calcola la distanza di tempo tra l'istante iniziale e quello finale.
		t_tot = t_end - t_start;

		if (id_proc == 0) {
			printf("\nApplicazione della strategia %d terminata in %e sec\n", strategia, t_tot);
			writeTimeCSV(test, strategia, n_proc, q_num, t_tot);
		}
	}

  	/* ************************************************************************ */
	// STAMPA DELL'OUTPUT

	/* ************************************************************************
	switch (strategia) {
		case NO_STRATEGY:
		case FIRST_STRATEGY:
		{
			if (id_proc == 0) {
				printf("\nLa somma totale e' %f\n", sum);
			}
			break;
		}
		case SECOND_STRATEGY:
		case THIRD_STRATEGY:
		{
			printf("\nProcesso n.%d\n", id_proc);
			printf("La somma totale e' %f\n", sum);
			break;
		}
		default:
			printf("Errore nella stampa dell'output!\n");
			break;
	}
	*/

  	/* ************************************************************************ */
	// TERMINAZIONE DELL'ESECUZIONE

	if (id_proc == 0) {
		printf("\nEsecuzione terminata.\n");
	}

	return 0;
}

/* **************************************************************************** */
/* RIFERIMENTI



*/
