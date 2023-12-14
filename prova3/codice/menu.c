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

	int rows = 0, cols = 0;
	int test = MULTIPLICATION_INPUT_TEST;
	int i = 0, pbs_count = 1;

	/*	*********************************************************************** */
	//	INTRODUZIONE

	printf("\n");
	printf("Benvenuto nell'applicazione di testing per le esercitazioni di\n");
	printf("Parallel and Distributed Computing A.A. 2023-2024\n\n");
	printTitle();

	/*	*********************************************************************** */
	//	APPLICAZIONE DELLA SUITE DI TESTING

	/*
		Nell'eseguire la suite di testing, il programma fornira':
		-	il risultato prodotto matrice-vettore;
		-	il tempo di esecuzione relativo all'esecuzione del prodotto.
	*/

	do {

		printf("Scegli un test da eseguire: \n");
		printf("%d. \t Prodotto matrice-vettore con scelta degli operandi.\n", MULTIPLICATION_INPUT_TEST);
		printf("%d. \t Prodotto matrice-vettore con input da file .csv.\n", MULTIPLICATION_CSV_TEST);
		printf("%d. \t Prodotto di una matrice per un vettore unitario.\n", MULTIPLICATION_ONE_TEST);
		printf("%d. \t Prodotto di una matrice per un vettore di numeri uguali.\n", MULTIPLICATION_SINGLE_NUMBER_TEST);
		printf("%d. \t Prodotto di una matrice per un autovettore.\n", MULTIPLICATION_EIGENVECTOR_TEST);
		printf("%d. \t Chiudere la suite di testing.\n\n", EXIT_TEST);
		test = getIntegerFromInput();
		checkScelta(test, MULTIPLICATION_INPUT_TEST, EXIT_TEST);

		switch(test) {
			case MULTIPLICATION_INPUT_TEST:
			case MULTIPLICATION_CSV_TEST:
			{

				/*	*************************************************************** */
				//	SCELTA DELLE DIMENSIONI DELLA MATRICE E DEL VETTORE

				printf("Inserisci la quantita' di righe della matrice:\n");
				rows = getIntegerFromInput();

				if (rows < 1) {
					printf("Devi inserire almeno una riga!\n");
					printf("Applicazione terminata.\n");
					exit(MATRIX_DIMENSION_ERROR);
				}

				printf("Inserisci la quantita' di colonne della matrice:\n");
				cols = getIntegerFromInput();

				if (cols < 1) {
					printf("Devi inserire almeno una colonna!\n");
					printf("Applicazione terminata.\n");
					exit(MATRIX_DIMENSION_ERROR);
				}

				createPBS(rows, cols, 1, test, OK_TIME_CALC, pbs_count++);
				createPBS(rows, cols, 2, test, OK_TIME_CALC, pbs_count++);
				createPBS(rows, cols, 4, test, OK_TIME_CALC, pbs_count++);
				createPBS(rows, cols, 7, test, OK_TIME_CALC, pbs_count++);
				createPBS(rows, cols, 8, test, OK_TIME_CALC, pbs_count++);

				break;
				
			}
			case MULTIPLICATION_ONE_TEST:
			case MULTIPLICATION_SINGLE_NUMBER_TEST:
			case MULTIPLICATION_EIGENVECTOR_TEST:
			{

				for (i = OP_MIN_EXP_TEST; i <= OP_MAX_EXP_TEST; i++) {
					
					rows = cols = pow(10, i);

					createPBS(rows, cols, 1, test, OK_TIME_CALC, pbs_count++);
					createPBS(rows, cols, 2, test, OK_TIME_CALC, pbs_count++);
					createPBS(rows, cols, 4, test, OK_TIME_CALC, pbs_count++);
					createPBS(rows, cols, 7, test, OK_TIME_CALC, pbs_count++);
					createPBS(rows, cols, 8, test, OK_TIME_CALC, pbs_count++);
				}
				
				break;
			}
			case EXIT_TEST:
			default:
				break;
		}

	} while (test != EXIT_TEST);

	/*	*********************************************************************** */
	//	USCITA DALL'APPLICATIVO

	printf("Applicazione terminata.\n");
	return 0;
}

/*	*************************************************************************** */
/*	RIFERIMENTI

*/
