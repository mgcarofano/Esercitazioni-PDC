/* **************************************************************************** */
// LIBRERIE

#include "./libraries/menufunc.h"

/* **************************************************************************** */

int main(int argc, char **argv) {

	/* ************************************************************************ */
	// DEFINIZIONE DELLE VARIABILI

	int scelta = 0, q_num = 0, time_calc = NO_TIME_CALC;
	int wait_time = TIMEOUT;

	FILE *out_file, *err_file;

	size_t err_size = 0;

	/* ************************************************************************ */
	// INTRODUZIONE

	printf("\n");
	printf("Benvenuto nell'applicazione di testing per le esercitazioni di\n");
	printf("Parallel and Distributed Computing A.A. 2023-2024\n\n");
	printTitle();

	/* ************************************************************************ */
	// SCELTA DELLA STRATEGIA DA APPLICARE

	printf("Scegli un'operazione da effettuare: \n");
	printf("%d. \t Applicazione della strategia 1.\n", FIRST_STRATEGY_APPLICATION);
	printf("%d. \t Applicazione della strategia 2.\n", SECOND_STRATEGY_APPLICATION);
	printf("%d. \t Applicazione della strategia 3.\n", THIRD_STRATEGY_APPLICATION);
	printf("%d. \t Esecuzione della suite di testing.\n", TESTING_SUITE);
	printf("%d. \t Chiudere l'applicazione.\n\n", EXIT_APPLICATION);
	scelta = getIntegerFromInput();
	checkScelta(scelta, 1, EXIT_APPLICATION);

	if (scelta != EXIT_APPLICATION) {

		/* ******************************************************************** */
		// APPLICAZIONE DEGLI ESEMPI D'USO

		if (scelta == TESTING_SUITE) {

			/*
				Nell'eseguire la suite di test, il programma fornira':
				-	il risultato della somma applicando la 1a strategia;
				-	il risultato della somma applicando la 2a strategia;
				-	il risultato della somma applicando la 3a strategia;
				-	i tempi di esecuzione relativi all'esecuzione
					di tutte e tre le strategie.
			*/

			printf("Scegli un test da eseguire: \n");
			printf("%d. \t Somma di 1.\n", SUM_ONE_TEST);
			printf("%d. \t Somma di 20 numeri con 20 processori.\n", SUM_20_NUMBERS_TEST);
			printf("%d. \t Somma dei primi 'N' numeri naturali.\n", GAUSS_TEST);
			printf("%d. \t Chiudere la suite di testing.\n\n", EXIT_TEST);
			scelta = getIntegerFromInput();
			checkScelta(scelta, 1, EXIT_TEST);

			// TODO: suite di testing
			// createPBS(scelta, q_num, OK_TIME_CALC);

		} else {

			/* **************************************************************** */

			printf("Inserisci la quantita' di numeri da sommare: \n");
			q_num = getIntegerFromInput();

			if (q_num <= 1) {
				printf("Devi inserire almeno due operandi!\n");
				printf("Applicazione terminata.\n");
				exit(NOT_ENOUGH_OPERANDS);
			}

			// CREAZIONE DEL FILE DI ESECUZIONE .PBS
			createPBS(scelta, q_num, NO_TIME_CALC);

			/* **************************************************************** */
			// ESECUZIONE DEL COMANDO QSUB

			printf("Esecuzione in corso...\n");

			/*
				--- int system (const char* command) ---
				Si utilizza questa funzione per eseguire
				un comando della shell. In particolare:
				-	mkdir:	per creare la directory dove memorizzare
							gli output dell'esecuzione.
				-	qsub:	per inviare il programma .pbs creato
							precedentemente alla coda di lavoro del cluster.
			*/

			system(MKDIR_PATH" -p ../output");
			system(QSUB_PATH" "NOME_PROVA".pbs > /dev/null 2>&1");

			printf("\n");

			/* **************************************************************** */
			// STAMPA DELL'OUTPUT

			printf("Stampa dell'output in corso...\n");

			/*
				Con questo ciclo while si attende che l'output venga
				scritto correttamente nei file .out e .err dall'esecuzione
				del comando 'qsub'.
			*/

			while (wait_time > 0) {
				if ((err_file = fopen("../output/"NOME_PROVA".err", "r")) == NULL
					|| (out_file = fopen("../output/"NOME_PROVA".out", "r")) == NULL) {
					
					sleep(1);
					printf("."); fflush(stdout);
					wait_time--;
				}
			}

			printf("\n\n");

			if (wait_time <= 0) {
				printf("Errore nella lettura dell'output!\n\n");
				printf("Applicazione terminata.\n");
				exit(FILE_OPENING_ERROR);
			}

			/*
				Si recupera la lunghezza del file .err leggendone il
				numero di byte:
				-	SEEK_END:	si sposta il puntatore alla fine del file.
				-	ftell():	ritorna la posizione corrente del puntatore.
				-	SEEK_SET:	si sposta il puntatore all'inizio del file
								per consentirne una successiva lettura.
			*/

			fseek(err_file, 0, SEEK_END); // seek to end of file
			err_size = ftell(err_file); // get current file pointer
			fseek(err_file, 0, SEEK_SET); // seek back to beginning of file

			/*
				--- void printFile(FILE *f) ---
				Si utilizza la funzione 'printFile' definita in 'menufunc.h'
				per riportare il contenuto del file 'f' sulla console.
				
				In particolare, se l'esecuzione del comando 'qsub' ha
				generato un file .err non vuoto, allora si preferisce la
				stampa di questo file al file .out.
			*/

			if (err_size > 0) {
				printf("\n\n---------- ERROR ----------\n\n");
				printFile(err_file);
				printf("\n\n------------\n");
			} else {
				printf("\n\n---------- OUTPUT ----------\n\n");
				printFile(out_file);
				printf("\n\n------------\n");
			}

			fclose(out_file);
			fclose(err_file);

		}

	}

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
