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

double getNumberFromInput() {
	double out = 0;
	char *buffer = NULL;
	size_t bufsize = 0;
	ssize_t chars_read;

	chars_read = getline(&buffer, &bufsize, stdin); // inizializzazione del buffer con caratteri estratti dallo stream di input
	printf("\n");

	if (chars_read < 0) {

		printf("Errore nella lettura dell'input!");
		printf("Applicazione terminata.\n");
		free(buffer);
		exit(1);

	} else {

		double digit_val = 0.0;
		int i = 0;
		int exp_whole = 0;
		int exp_fract = 0;
		int pos_point = -1;

		// si inizia da chars_read-2 perché:
		// -	la numerazione del buffer è da 0 a N-1
		// -	l'ultimo carattere è \0

		for (i = chars_read-2; i >= 0; i--) {
			if (buffer[i] != '.' && buffer[i] != ',') {
				exp_fract--;
			} else {
				pos_point = chars_read - (exp_fract * (-1)) -2;
				break;
			}
		}

		// printf("--- chars_read: %zd ---\n", chars_read);
		// printf("--- pos_point: %d ---\n\n", pos_point);

		i = chars_read-2;
		while(i >= 0) {
			// printf("--- i: %d ---\n", i);
			// printf("--- buffer[i]: %c ---\n", buffer[i]);
			// printf("--- exp_whole: %d ---\n", exp_whole);
			// printf("--- exp_fract: %d ---\n", exp_fract);

			if (i > pos_point && pos_point != -1) {

				if (isdigit(buffer[i])) {
					// printf("--- pow(10, exp_fract): %f ---\n", pow(10, exp_fract));
					// printf("--- digit_val: %f ---\n\n", digit_val);
					digit_val = pow(10, exp_fract) * (digit_val + (buffer[i] - '0')); // per ottenere il valore intero del carattere ASCII
					out = out + digit_val;
					// printf("--- out: %f ---\n", out);
					// printf("--- digit_val: %f ---\n\n", digit_val);
				} else {
					out = 0.0;
					printf("Puoi inserire solo valori numerici reali!\n");
					printf("Applicazione terminata.\n");
					free(buffer);
					exit(1);
				}

				exp_fract++;

			} else if (i < pos_point || pos_point == -1) {

				if (isdigit(buffer[i])) {
					// printf("--- pow(10, exp_fract): %f ---\n", pow(10, exp_fract));
					// printf("--- digit_val: %f ---\n\n", digit_val);
					digit_val = pow(10, exp_whole) * (digit_val + (buffer[i] - '0'));
					out = out + digit_val;
					// printf("--- out: %f ---\n", out);
					// printf("--- digit_val: %f ---\n\n", digit_val);
				} else {
					if (i == 0 && buffer[0] == '-') {
						out = out * (-1);
					} else {
						out = 0.0;
						printf("Puoi inserire solo valori numerici reali!\n");
						printf("Applicazione terminata.\n");
						free(buffer);
						exit(1);
					}
				}

				exp_whole++;
			}

			digit_val = 0.0;
			i--;
		}
	}

	free(buffer);
	return out;
}

int getIntegerFromInput() {

	double out_double = 0.0;
	int out_integer = 0;
	
	out_double = getNumberFromInput();
	out_integer = (int)out_double;

	if (out_integer < 0) {
		printf("Puoi inserire solo valori numerici naturali!\n");
		printf("Applicazione terminata.\n");
		exit(1);
	}

	return out_integer;
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

	int scelta = 0, count = 0, q_num = 0, time_calc = 0;
	double op = 0.0;
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
	printf("%d. \t Chiudere l'applicazione.\n\n", ++count);
  	scelta = getIntegerFromInput();
	checkScelta(scelta, 1, count);

	/* ************************************************************************ */

	if (scelta != 5) {

		printf("Inserisci la quantita' di numeri da sommare: \n");
		q_num = getIntegerFromInput();

		if (q_num <= 1) {
			printf("Devi inserire almeno due operandi!\n");
			printf("Applicazione terminata.\n");
			exit(1);
		}

		if (scelta != 4) {
			printf("Vuoi calcolare anche i tempi di esecuzione? (no = 0, si = 1)\n");
			time_calc = getIntegerFromInput();
			checkScelta(time_calc, 0, 1);
		}

		/* ******************************************************************** */
		// CREAZIONE DEL FILE DI ESECUZIONE .PBS

		if ((pbs_file = fopen(NOME_PROVA".pbs", "w")) == NULL) {
			printf("Errore durante l'esecuzione!");
			printf("Applicazione terminata.\n");
			exit(1);
		}

		fprintf(pbs_file,
					"#!/bin/bash\n"
					"\n"
					"#PBS -q studenti\n"
		);

		fclose(pbs_file);

		if ((pbs_file = fopen(NOME_PROVA".pbs", "a")) == NULL) {
			printf("Errore durante l'esecuzione!");
			printf("Applicazione terminata.\n");
			exit(1);
		}

		fprintf(pbs_file, "#PBS -l nodes="NODE_NUMBER);

		if (time_calc == 1) {
			fprintf(pbs_file, ":ppn="NODE_NUMBER);
		}
		
		fprintf(pbs_file,
					"\n#PBS -N " NOME_PROVA "\n"
					"#PBS -o ../output/" NOME_PROVA ".out\n"
					"#PBS -e ../output/" NOME_PROVA ".err\n"
					"\n"
					"echo --- \n"
		);

		if (time_calc == 1) {
			fprintf(pbs_file, "sort -u $PBS_NODEFILE > hostlist\n");
			fprintf(pbs_file, "NCPU=$(wc -l < hostlist)\n");
		} else {
			fprintf(pbs_file, "NCPU=$(wc -l < $PBS_NODEFILE)\n");
		}
		
		fprintf(pbs_file,
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
		);

		if (time_calc == 1) {
			fprintf(pbs_file, "-machinefile hostlist -np ${NCPU} ");
		} else {
			fprintf(pbs_file, "-machinefile $PBS_NODEFILE -n ${NCPU} ");
		}

		fprintf(pbs_file, "$PBS_O_WORKDIR/" NOME_PROVA " %d %d", scelta, q_num);

		if (scelta != 4 && q_num <= 20) {
			int i = 1;
			for (i = 1; i <= q_num; i++) {
				printf("Inserisci il %do operando da sommare: \n", i);
				op = getNumberFromInput();
				fprintf(pbs_file, " %f", op);
			}
		}

		fprintf(pbs_file,
					"\necho ---\n"
					"echo PBS: Job completato.\n"
					"echo --- \n"
		);

		fclose(pbs_file);

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
