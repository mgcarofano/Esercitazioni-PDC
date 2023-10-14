/* ************************************************************************** */
// LIBRERIE

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <fcntl.h>
#include <errno.h>

/* ************************************************************************** */
// ENUMERAZIONI E COSTANTI

#define NOME_PROVA "prova1"
#define NODE_NUMBER "8"
#define QSUB_PATH "/usr/bin/qsub"
#define MKDIR_PATH "/bin/mkdir"

/* ************************************************************************** */
// DEFINIZIONE DI FUNZIONI E TIPI ACCESSORI

void printTitle() {
	printf("################################################################\n");
	printf("#      ________      __                     __           ____  #\n");
	printf("#     / ____/ /___ _/ /_  ____  _________ _/ /_____     /  _/  #\n");
	printf("#    / __/ / / __ `/ __ \\/ __ \\/ ___/ __ `/ __/ __ \\    / /    #\n");
	printf("#   / /___/ / /_/ / /_/ / /_/ / /  / /_/ / /_/ /_/ /  _/ /     #\n");
	printf("#  /_____/_/\\__,_/_.___/\\____/_/   \\__,_/\\__/\\____/  /___/     #\n");
	printf("#                                                              #\n");
	printf("# ---------- Calcolo della somma di N numeri reali ----------- #\n");
	printf("#                                                              #\n");
	printf("################################################################\n");
	printf("\n");
}

int getIntegerFromInput() {
	int scelta_intero = 0, lim_inf = 1;
	
	char *buffer = NULL;
	size_t bufsize = 0;
	ssize_t chars_read;

	chars_read = getline(&buffer, &bufsize, stdin); // inizializzazione del buffer con caratteri estratti dallo stream di input
	
	if (chars_read < 0) {

		printf("Errore nella lettura dell'input!");
		printf("Applicazione terminata.\n");
		free(buffer);
		exit(1);

	} else {

		int digit_val = 0;
		int i = 0;
		int exp = 0;

		// printf("--- chars_read: %zd ---\n\n", chars_read);

		for (i = chars_read-2; i >= 0; i--) {
			// printf("--- buffer[i]: %c ---\n", buffer[i]);
			// printf("--- exp: %d ---\n", exp);
			// printf("--- pow(10, exp): %f ---\n", pow(10, exp));

			if (isdigit(buffer[i])) {
				// printf("--- digit_val: %d ---\n", digit_val);
				digit_val = pow(10, exp) * (digit_val + (buffer[i] - '0')); // per ottenere il valore intero del carattere ASCII
				scelta_intero = scelta_intero + digit_val;
				// printf("--- scelta_intero: %d ---\n", digit_val);
				// printf("--- digit_val: %d ---\n\n", digit_val);
			} else {
				scelta_intero = 0;
				printf("Puoi inserire solo valori numerici!\n");
				printf("Applicazione terminata.\n");
				free(buffer);
				exit(1);
			}

			digit_val = 0;
			exp++;
		}
	}

	free(buffer);
	return scelta_intero;
}

void checkScelta(int scelta, int lim_inf, int lim_sup) {
	if(!(scelta >= lim_inf && scelta <= lim_sup)) {
		printf("Puoi inserire solo un valore numerico intero compreso tra %d e %d!\n", lim_inf, lim_sup);
		printf("Applicazione terminata.\n");
		exit(1);
	}

	return;
}

void printFile(FILE *f) {
	char char_to_read;

	do {
		char_to_read = fgetc(f);
		if (char_to_read != EOF) printf("%c", char_to_read);
	} while (char_to_read != EOF);
}


/* ************************************************************************** */

int main(int argc, char const *argv[]) {

	/* ************************************************************************ */
	// DEFINIZIONE DELLE VARIABILI

	int scelta = 0, count = 0, q_num = 0;
	FILE *pbs_file, *qsub_out, *out_file, *err_file;
	char char_to_read, *wait_input = NULL;
	size_t err_size = 0, buf_size = 0;

	/* ************************************************************************ */
	// INTRODUZIONE

	printf("\n");
	printf("Benvenuto nell'applicazione di testing per le esercitazioni di\n");
	printf("Parallel and Distributed Computing A.A. 2023-2024\n\n");
	printTitle();

	/* ************************************************************************ */

	printf("Scegli un'operazione da effettuare: \n");
	printf("%d. \t Applicazione della strategia 1.\n", ++count);
	printf("%d. \t Applicazione della strategia 2.\n", ++count);
	printf("%d. \t Applicazione della strategia 3.\n", ++count);
	printf("%d. \t Esecuzione dell'esempio d'uso (somma di 1).\n", ++count);
	printf("%d. \t Calcolo dei tempi di esecuzione.\n", ++count);
	printf("%d. \t Chiudere l'applicazione.\n\n", ++count);
  	scelta = getIntegerFromInput();
	checkScelta(scelta, 1, count);

	printf("\n");

	/* ************************************************************************ */

	if (scelta != 6) {

		printf("Inserisci la quantita' di numeri da sommare: \n");
		q_num = getIntegerFromInput();

		// printf("q_num: %d\n", q_num);

		if (q_num < 0) {
			printf("Puoi inserire solo un valore numerico intero maggiore o uguale a 0!\n");
			printf("Applicazione terminata.\n");
			exit(1);
		}

		printf("\n");

		/* ******************************************************************** */
		// CREAZIONE DEL FILE DI ESECUZIONE .PBS

		printf("Preparazione dell'output in corso...\n");

		if ((pbs_file = fopen(NOME_PROVA".pbs", "w")) == NULL) {
			printf("Errore durante l'esecuzione!");
			printf("Applicazione terminata.\n");
			exit(1);
		}

		fprintf(pbs_file,
					"#!/bin/bash\n"
					"\n"
					"#PBS -q studenti\n"
					"#PBS -l nodes=" NODE_NUMBER "\n"
					"#PBS -N " NOME_PROVA "\n"
					"#PBS -o ../output/" NOME_PROVA ".out\n"
					"#PBS -e ../output/" NOME_PROVA ".err\n"
					"\n"
					"echo --- \n"
					"NCPU=$(wc -l < $PBS_NODEFILE)\n"
					"PBS_O_WORKDIR=$PBS_O_HOME/" NOME_PROVA "/codice\n"
					"\n"
					"echo PBS: la directory di lavoro e\\' $PBS_O_WORKDIR\n"
					"echo PBS: Compilazione in esecuzione...\n"
					"/usr/lib64/openmpi/1.4-gcc/bin/mpicc "
					"-o $PBS_O_WORKDIR/" NOME_PROVA " $PBS_O_WORKDIR/" NOME_PROVA ".c\n"
					"echo PBS: Compilazione completata.\n"
					"\n"
					"echo 'PBS: Job in esecuzione su '${NCPU}' cpu...'\n"
					"echo ---\n"
					"/usr/lib64/openmpi/1.4-gcc/bin/mpiexec "
					"-machinefile $PBS_NODEFILE -n ${NCPU} $PBS_O_WORKDIR/" NOME_PROVA " %d %d\n"
					"echo ---\n"
					"echo PBS: Job completato.\n"
					"echo --- \n",
		scelta, q_num);

		// TODO: aggiungere controllo scelta == 5 (per calcolare i tempi serve un .pbs diverso)
		// TODO: aggiungere controllo q_num <=20 con input da tastiera

		fclose(pbs_file);
		printf("\n");

		/* ******************************************************************** */
		// ESECUZIONE DEL COMANDO QSUB

		printf("Esecuzione in corso...\n");

		system(MKDIR_PATH" -p ../output");
		system(QSUB_PATH" "NOME_PROVA".pbs > /dev/null 2>&1");

		printf("Premi il tasto invio per continuare.\n");
		getline(&wait_input, &buf_size, stdin);

		printf("\n");

		/* ******************************************************************** */
		// STAMPA DELL'OUTPUT

		printf("Stampa dell'output in corso...\n");

		if ((err_file = fopen("../output/"NOME_PROVA".err", "r")) == NULL) {
			printf("Errore nella lettura dell'output!\n\n");
			printf("Applicazione terminata.\n");
			exit(1);
		}

		if ((out_file = fopen("../output/"NOME_PROVA".out", "r")) == NULL) {
			printf("Errore nella lettura dell'output!\n\n");
			printf("Applicazione terminata.\n");
			exit(1);
		}

		fseek(err_file, 0, SEEK_END); // seek to end of file
		err_size = ftell(err_file); // get current file pointer
		fseek(err_file, 0, SEEK_SET); // seek back to beginning of file

		if (err_size > 0) {

			printf("\n\n--- ERROR ---\n\n");
			printFile(err_file);
			printf("\n\n-----\n");

		} else {

			printf("\n\n--- OUTPUT ---\n\n");
			printFile(out_file);
			printf("\n\n-----\n");

		}

		fclose(out_file);
		fclose(err_file);
		printf("\n");

	}

	/* ************************************************************************ */
	// USCITA DALL'APPLICATIVO

	printf("Applicazione terminata.\n");
	return 0;
}

/* ************************************************************************** */

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
