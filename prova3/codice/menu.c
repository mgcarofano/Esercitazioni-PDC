/*

	menu.c
	di Mario Gabriele Carofano
	e Francesco Noviello

*/

/*	*************************************************************************** */
//	LIBRERIE

#include "./libraries/menufunc.h"

/*	*************************************************************************** */

int main(int argc, char **argv) {

	/*	*********************************************************************** */
	//	INIZIALIZZAZIONE DELL'AMBIENTE DI LAVORO

	int mat_dim = 0;
	int A_rows = 0, A_cols = 0;
	int B_rows = 0, B_cols = 0;
	int scelta = DEFAULT_SCELTA, input = DEFAULT_INPUT, test = DEFAULT_TEST;
	int i = 0, pbs_count = 1;

	/*	*********************************************************************** */
	//	INTRODUZIONE

	printf("\n");
	printf("Benvenuto nell'applicazione di testing per le esercitazioni di\n");
	printf("Parallel and Distributed Computing A.A. 2023-2024\n\n");
	printTitle();

	do {

		/*	******************************************************************* */
		//	SCELTA DELL'OPERAZIONE DA EFFETTUARE

		printf("Scegli un'operazione da effettuare: \n");
		printf("%d. \t Calcolo del prodotto matrice-matrice senza test.\n", NO_TEST);
		printf("%d. \t Esecuzione della suite di testing.\n", TESTING_SUITE);
		printf("%d. \t Chiudere l'applicazione.\n\n", EXIT_APPLICATION);
		scelta = getIntegerFromInput();
		checkScelta(scelta, NO_TEST, EXIT_APPLICATION);

		/*	******************************************************************* */

		switch(scelta) {
			case NO_TEST:
			{
				/*	*********************************************************** */
				//	SCELTA DELLA DIMENSIONE DELLE MATRICI

				printf("Inserisci la dimensione delle matrici:\n");
				mat_dim = getIntegerFromInput();

				if (mat_dim < 1) {
					printf("Devi inserire una dimensione maggiore di 1!\n");
					printf("Applicazione terminata.\n");
					exit(MATRIX_DIMENSION_ERROR);
				}

				A_rows = A_cols = B_rows = B_cols = mat_dim;

				/*	*********************************************************** */
				//	SCELTA DELL'INPUT

				printf("Scegli il tipo di input: \n");
				printf("%d. \t Input da linea di comando (fino a %d elementi).\n", VALUES_FROM_INPUT, OP_MAX_QUANTITY);
				printf("%d. \t Input da file .csv.\n", VALUES_FROM_CSV);
				input = getIntegerFromInput();

				/* ************************************************************ */
				// CREAZIONE DEL FILE DI ESECUZIONE .PBS

				createPBS(A_rows, A_cols, B_rows, B_cols, 4, input, DEFAULT_TEST, NO_TIME_CALC, pbs_count++);

				break;
			}
			case TESTING_SUITE:
			{
				/*	*********************************************************** */
				//	APPLICAZIONE DELLA SUITE DI TESTING

				/*
					Nell'eseguire la suite di testing, il programma fornira':
					-	il risultato del prodotto matrice-matrice;
					-	il tempo di esecuzione relativo all'esecuzione
						del solo prodotto.
					-	un valore booleano che indica se il risultato in
						output e' corretto, in base al test scelto.
				*/

				printf("Scegli un test da eseguire: \n");
				printf("%d. \t Calcolo del prodotto con la matrice identita'.\n", MULTIPLICATION_IDENTITY_TEST);
				printf("%d. \t Confronto di T(AB) con T(B)•T(A), dove T(-) e' la matrice trasposta.\n", MULTIPLICATION_TRANSPOSE_TEST);
				printf("%d. \t Confronto di tr(AB) e tr(BA), dove tr(-) e' la traccia della matrice.\n", MULTIPLICATION_TRACE_TEST);
				printf("%d. \t Chiudere la suite di testing.\n\n", EXIT_TEST);
				test = getIntegerFromInput();
				checkScelta(test, MULTIPLICATION_IDENTITY_TEST, EXIT_TEST);

				if (test != EXIT_TEST) {
					for (i = MIN_COEFF_TEST; i <= MAX_COEFF_TEST; i++) {

						/*	*************************************************** */
						//	SCELTA DELLA DIMENSIONE DELLE MATRICI

						A_rows = A_cols = B_rows = B_cols = 4 * 9 * i;

						/* **************************************************** */
						// CREAZIONE DEL FILE DI ESECUZIONE .PBS

						createPBS(A_rows, A_cols, B_rows, B_cols, 1, DEFAULT_INPUT, test, OK_TIME_CALC, pbs_count++);
						createPBS(A_rows, A_cols, B_rows, B_cols, 4, DEFAULT_INPUT, test, OK_TIME_CALC, pbs_count++);
						createPBS(A_rows, A_cols, B_rows, B_cols, 9, DEFAULT_INPUT, test, OK_TIME_CALC, pbs_count++);
					}
				}

				break;
			}
			case EXIT_APPLICATION:
			default:
				break;
		}

	} while (scelta != EXIT_APPLICATION);

	/*	*********************************************************************** */
	//	USCITA DALL'APPLICATIVO

	printf("Applicazione terminata.\n");
	return 0;
}

/*	*************************************************************************** */
/*	RIFERIMENTI

*/
