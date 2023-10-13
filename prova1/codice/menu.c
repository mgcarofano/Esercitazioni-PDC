/* ************************************************************************** */
// LIBRERIE

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

/* ************************************************************************** */
// ENUMERAZIONI E COSTANTI

#define NOME_PROVA "prova1"
#define NODE_NUMBER "8"

/* ************************************************************************** */
// DEFINIZIONE DI FUNZIONI E TIPI ACCESSORI

int getIntegerFromInput() {
	int scelta_intero = 0, lim_inf = 1;
	
	char *buffer = NULL;
	size_t bufsize = 0;
	ssize_t chars_read;

	// buffer = malloc(bufsize * sizeof(char));
	// if(buffer == NULL) {
	// 	printf("Errore di allocazione della memoria!");
	// 	exit(1);
	// }

	chars_read = getline(&buffer, &bufsize, stdin); // inizializzazione del buffer con caratteri estratti dallo stream di input
	
	if (chars_read < 0) {

		printf("Errore nella lettura dell'input!");
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

/* ************************************************************************** */

int main(int argc, char const *argv[]) {

	/* ************************************************************************ */
	// DEFINIZIONE DELLE VARIABILI

	int scelta = 0, cont = 0, q_num = 0;
	FILE *pbs_file;

	/* ************************************************************************ */
	// INTRODUZIONE

	printf("\n");
	printf("Buongiorno! Benvenuto nell'applicazione di testing per le\n");
	printf("esercitazioni di Parallel and Distributed Computing 2023-2024\n\n");

	/* ************************************************************************ */

	printf("Scegli un'operazione da effettuare: \n");
	printf("%d. \t Applicazione della strategia 1.\n", ++cont);
	printf("%d. \t Applicazione della strategia 2.\n", ++cont);
	printf("%d. \t Applicazione della strategia 3.\n", ++cont);
	printf("%d. \t Esecuzione dell'esempio d'uso (somma di 1).\n", ++cont);
	printf("%d. \t Chiudere l'applicazione.\n", ++cont);
  	scelta = getIntegerFromInput();
	checkScelta(scelta, 1, cont);

	/* ************************************************************************ */

	if (scelta != 5) {

		printf("Inserisci la quantita' di numeri da sommare: \n");
		q_num = getIntegerFromInput();

		// printf("q_num: %d\n", q_num);

		if (q_num < 0) {
			printf("Puoi inserire solo un valore numerico intero maggiore o uguale a 0!\n");
			printf("Applicazione terminata.\n");
			exit(1);
		}

		/* ******************************************************************** */
		// CREAZIONE DEL FILE DI ESECUZIONE .PBS

		pbs_file = fopen(NOME_PROVA".pbs", "w");

		fprintf(pbs_file,
					"#!/bin/bash\n"
					"\n"
					"#PBS -q studenti\n"
					"#PBS -l nodes=" NODE_NUMBER "\n"
					"#PBS -N " NOME_PROVA "\n"
					"#PBS -o " NOME_PROVA ".out\n"
					"#PBS -e " NOME_PROVA ".err\n"
					"\n"
					"echo --- \n"
					"PBS_NODENUMBER=" NODE_NUMBER "\n"
					"PBS_O_WORKDIR=$PBS_O_HOME/" NOME_PROVA "/codice\n"
					"\n"
					"echo Compilazione in esecuzione\n"
					"/usr/lib64/openmpi/1.4-gcc/bin/mpicc"
					"-o $PBS_O_WORKDIR/" NOME_PROVA " $PBS_O_WORKDIR/" NOME_PROVA ".c\n"
					"\n"
					"echo Job in esecuzione\n"
					"/usr/lib64/openmpi/1.4-gcc/bin/mpiexec"
					"-machinefile $PBS_NODEFILE -n $PBS_NODENUMBER $PBS_O_WORKDIR/" NOME_PROVA " %d %d", scelta, q_num
		);

		fclose(pbs_file);

		/* ******************************************************************** */
		// ESECUZIONE DEL COMANDO QSUB



		/* ******************************************************************** */
		// STAMPA DELL'OUTPUT



	}

	/* ************************************************************************ */
	// USCITA DALL'APPLICATIVO

	printf("Applicazione terminata.\n");
	return 0;
}

/* ************************************************************************** */

/* RIFERIMENTI

https://stackoverflow.com/questions/35890237/how-to-create-a-type-definition-for-a-string-in-c
https://stackoverflow.com/questions/5029840/convert-char-to-int-in-c-and-c
https://opensource.com/article/22/5/safely-read-user-input-getline
https://stackoverflow.com/questions/19209141/how-do-i-execute-a-shell-built-in-command-with-a-c-function
https://www.w3schools.com/c/c_files_write.php
https://stackoverflow.com/questions/5256313/c-c-macro-string-concatenation

*/
