/*

	menu.c
	di Mario Gabriele Carofano
	e Francesco Noviello

*/

/* **************************************************************************** */
// LIBRERIE

#include "./libraries/menufunc.h"

/* **************************************************************************** */

int main(int argc, char **argv) {

	/* ************************************************************************ */
	// INIZIALIZZAZIONE DELL'AMBIENTE DI LAVORO

	int strategia = NO_STRATEGY, q_num = 0;
	int test = NO_TEST, time_calc = NO_TIME_CALC;

	/* ************************************************************************ */
	// INTRODUZIONE

	printf("\n");
	printf("Benvenuto nell'applicazione di testing per le esercitazioni di\n");
	printf("Parallel and Distributed Computing A.A. 2023-2024\n\n");
	printTitle();

	/* ************************************************************************ */
	// SCELTA DELLA STRATEGIA DA APPLICARE

	do {

		printf("Scegli un'operazione da effettuare: \n");
		printf("%d. \t Chiudere l'applicazione.\n\n", EXIT_APPLICATION);
		strategia = getIntegerFromInput();
		checkScelta(strategia, EXIT_APPLICATION, EXIT_APPLICATION);

		/* **************************************************************** */
		// SCELTA DEL NUMERO DI OPERANDI DA SOMMARE

		printf("Inserisci la quantita' di numeri da sommare: \n");
		q_num = getIntegerFromInput();

		if (q_num <= 1) {
			printf("Devi inserire almeno due operandi!\n");
			printf("Applicazione terminata.\n");
			exit(NOT_ENOUGH_OPERANDS);
		}

		/* **************************************************************** */
		// APPLICAZIONE DELLA SUITE DI TESTING

	} while (strategia != EXIT_APPLICATION);

	/* ************************************************************************ */
	// USCITA DALL'APPLICATIVO

	printf("Applicazione terminata.\n");
	return 0;
}

/* **************************************************************************** */
/* RIFERIMENTI

*/
