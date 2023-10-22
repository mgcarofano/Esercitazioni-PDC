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

	int strategia = NO_STRATEGY, q_num = 0, test = NO_TEST, time_calc = NO_TIME_CALC;
	int i = 0, pbs_count = 1;
	
	FILE *pbs_file;

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
		printf("%d. \t Applicazione della strategia 1.\n", FIRST_STRATEGY);
		printf("%d. \t Applicazione della strategia 2.\n", SECOND_STRATEGY);
		printf("%d. \t Applicazione della strategia 3.\n", THIRD_STRATEGY);
		printf("%d. \t Esecuzione della suite di testing.\n", TESTING_SUITE);
		printf("%d. \t Chiudere l'applicazione.\n\n", EXIT_APPLICATION);
		strategia = getIntegerFromInput();
		checkScelta(strategia, FIRST_STRATEGY, EXIT_APPLICATION);

		if (strategia <= THIRD_STRATEGY) {

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
			// CREAZIONE DEL FILE DI ESECUZIONE .PBS

			createPBS(8, strategia, q_num, NO_TEST, NO_TIME_CALC, pbs_count++);

		/* ******************************************************************** */
		// APPLICAZIONE DELLA SUITE DI TESTING

		} else if (strategia == TESTING_SUITE) {

			/*
				Nell'eseguire la suite di testing, il programma fornira':
				-	il risultato della somma applicando la 1a strategia;
				-	il risultato della somma applicando la 2a strategia (se possibile);
				-	il risultato della somma applicando la 3a strategia (se possibile);
				-	i tempi di esecuzione relativi all'esecuzione delle 3 strategie.
			*/

			printf("Scegli un test da eseguire: \n");
			printf("%d. \t Somma di 1.\n", SUM_ONE_TEST);
			printf("%d. \t Somma di un singolo numero.\n", SUM_SINGLE_NUMBER_TEST);
			printf("%d. \t Somma di numeri interi opposti.\n", SUM_OPPOSITE_NUMBER_TEST);
			printf("%d. \t Somma di 'N' numeri naturali di un intervallo.\n", GAUSS_TEST);
			printf("%d. \t Chiudere la suite di testing.\n\n", EXIT_TEST);
			test = getIntegerFromInput();
			checkScelta(test, SUM_ONE_TEST, EXIT_TEST);

			if (test != EXIT_TEST) {
				for (strategia = FIRST_STRATEGY; strategia <= THIRD_STRATEGY; strategia++) {
					for (i = OP_MIN_EXP_TEST; i <= OP_MAX_EXP_TEST; i++) {

						q_num = pow(10, i);

						createPBS(1, strategia, q_num, test, OK_TIME_CALC, pbs_count++);
						createPBS(2, strategia, q_num, test, OK_TIME_CALC, pbs_count++);
						createPBS(4, strategia, q_num, test, OK_TIME_CALC, pbs_count++);
						createPBS(7, strategia, q_num, test, OK_TIME_CALC, pbs_count++);
						createPBS(8, strategia, q_num, test, OK_TIME_CALC, pbs_count++);
					}
				}
			}
		}

	} while (strategia != EXIT_APPLICATION);

	/* ************************************************************************ */
	// USCITA DALL'APPLICATIVO

	printf("Applicazione terminata.\n");
	return 0;
}

/* **************************************************************************** */

/* RIFERIMENTI

https://patorjk.com/software/taag/#p=display&f=Slant&t=Elaborato%20I

https://stackoverflow.com/questions/35890237/how-to-create-a-type-definition-for-a-string-in-c
https://stackoverflow.com/questions/5029840/convert-char-to-int-in-c-and-c
https://opensource.com/article/22/5/safely-read-user-input-getline

https://stackoverflow.com/questions/19209141/how-do-i-execute-a-shell-built-in-command-with-a-c-function
https://kb.iu.edu/d/acec
https://linuxhint.com/exec_linux_system_call_c/
https://www.cs.uleth.ca/~holzmann/C/system/shell_commands.html

https://www.w3schools.com/c/c_files_write.php
https://stackoverflow.com/questions/5256313/c-c-macro-string-concatenation
https://www.geeksforgeeks.org/c-program-print-contents-file/

https://stackoverflow.com/questions/8671366/undefined-reference-to-pow-and-floor

*/
