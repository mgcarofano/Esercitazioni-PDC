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
	// DEFINIZIONE DELLE VARIABILI

	int scelta = 0, q_num = 0, time_calc = NO_TIME_CALC;
	int pbs_count = 0;
	
	char *pbs_path;
	FILE *pbs_file, *out_file, *err_file;

	size_t err_size = 0;

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
		scelta = getIntegerFromInput();
		checkScelta(scelta, 1, EXIT_APPLICATION);

		/* ******************************************************************** */
		// APPLICAZIONE DEGLI ESEMPI D'USO

		if (scelta == TESTING_SUITE) {

			/*
				Nell'eseguire la suite di testing, il programma fornira':
				-	il risultato della somma applicando la 1a strategia;
				-	il risultato della somma applicando la 2a strategia (se possibile);
				-	il risultato della somma applicando la 3a strategia (se possibile);
				-	i tempi di esecuzione relativi all'esecuzione delle 3 strategie.
			*/

			printf("Scegli un test da eseguire: \n");
			printf("%d. \t Somma di 1.\n", SUM_ONE_TEST);
			printf("%d. \t Somma di 20 numeri con 20 processori.\n", SUM_20_NUMBERS_TEST);
			printf("%d. \t Somma dei primi 'N' numeri naturali.\n", GAUSS_TEST);
			printf("%d. \t Chiudere la suite di testing.\n\n", EXIT_TEST);
			scelta = getIntegerFromInput();
			checkScelta(scelta, 1, EXIT_TEST);

			switch(scelta) {
				case SUM_ONE_TEST: {
					break;
				}
				case SUM_20_NUMBERS_TEST: {
					break;
				}
				case GAUSS_TEST: {
					printf("Inserisci il limite superiore dell'intervallo: \n");
					q_num = getIntegerFromInput();

					if (q_num <= 1) {
						printf("Devi inserire almeno due operandi!\n");
						printf("Applicazione terminata.\n");
						exit(NOT_ENOUGH_OPERANDS);
					}
		
					// createPBS(...)
					
					break;
				}
				case EXIT_TEST:
				default:
					break;
				
			}

		} else if (scelta != EXIT_APPLICATION) {

			/* **************************************************************** */

			printf("Inserisci la quantita' di numeri da sommare: \n");
			q_num = getIntegerFromInput();

			if (q_num <= 1) {
				printf("Devi inserire almeno due operandi!\n");
				printf("Applicazione terminata.\n");
				exit(NOT_ENOUGH_OPERANDS);
			}

			/* **************************************************************** */
			// CREAZIONE DEL FILE DI ESECUZIONE .PBS

			if (pbs_count == 0) {
				system(RM_PATH" -rf ../jobs");
				system(MKDIR_PATH" -p ../jobs");
			}

			sprintf(pbs_path, "../jobs/"NOME_PROVA"_%d.pbs", ++pbs_count);
			createPBS(8, scelta, q_num, NO_TEST, NO_TIME_CALC, pbs_path, pbs_count);
			printf("%s creato con successo!\n\n", pbs_path);

		}

	} while (scelta != EXIT_APPLICATION);

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
