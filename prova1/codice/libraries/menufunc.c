/*

	menufunc.c
	di Mario Gabriele Carofano
	e Francesco Noviello

*/

/* **************************************************************************** */
// LIBRERIE

#include "constants.c"

#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>

/* **************************************************************************** */
// CODICE DELLE FUNZIONI DEFINITE IN 'menufunc.h'

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
		exit(INPUT_LINE_ERROR);

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
					exit(NOT_REAL_NUMBER_ERROR);
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
						exit(NOT_REAL_NUMBER_ERROR);
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
		exit(NOT_NATURAL_NUMBER_ERROR);
	}

	return out_integer;
}

void checkScelta(int scelta, int lim_inf, int lim_sup) {
	if(!(scelta >= lim_inf && scelta <= lim_sup)) {
		printf("Puoi inserire solo un valore numerico intero compreso tra %d e %d!\n", lim_inf, lim_sup);
		printf("Applicazione terminata.\n");
		exit(NOT_IN_RANGE_ERROR);
	}
}

void createPBS(int n_proc, int scelta, int q_num, int test, int time_calc, const char* path, int pbs_count) {

	FILE *pbs_file;
	double op = 0.0;

	if ((pbs_file = fopen(path, "w")) == NULL) {
		printf("Errore durante l'esecuzione!\n");
		printf("Applicazione terminata.\n");
		exit(FILE_OPENING_ERROR);
	}

	fprintf(pbs_file, "#!/bin/bash\n");
	fclose(pbs_file);

	if ((pbs_file = fopen(path, "a")) == NULL) {
		printf("Errore durante l'esecuzione!\n");
		printf("Applicazione terminata.\n");
		exit(FILE_OPENING_ERROR);
	}

	fprintf(pbs_file,
				"\n"
				"#PBS -q studenti\n"
				"#PBS -l nodes="NODE_NUMBER);

	if (time_calc == OK_TIME_CALC) {
		fprintf(pbs_file, ":ppn="NODE_PROCESS);
	}
	
	fprintf(pbs_file,
				"\n#PBS -N " NOME_PROVA "_%d\n"
				"#PBS -o ../output/" NOME_PROVA "_%d.out\n"
				"#PBS -e ../output/" NOME_PROVA "_%d.err\n"
				"\n"
				"rm -fr $PBS_O_HOME/prova1/output\n"
				"mkdir -p $PBS_O_HOME/prova1/output\n"
				"\n"
				"echo --- \n",
	pbs_count, pbs_count, pbs_count);

	if (time_calc == OK_TIME_CALC) {
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
				"-o $PBS_O_WORKDIR/" NOME_PROVA "_%d -lm $PBS_O_WORKDIR/" NOME_PROVA ".c\n"
				"echo PBS: Compilazione completata.\n"
				"\n"
				"echo 'PBS: Job in esecuzione su '${NCPU}' cpu...'\n"
				"echo '>>>'\n"
				"/usr/lib64/openmpi/1.4-gcc/bin/mpiexec ",
	pbs_count);

	if (time_calc == OK_TIME_CALC) {
		fprintf(pbs_file, "-machinefile hostlist -np ${NCPU} ");
	} else {
		fprintf(pbs_file, "-machinefile $PBS_NODEFILE -n ${NCPU} ");
	}

	fprintf(pbs_file, "$PBS_O_WORKDIR/" NOME_PROVA "_%d %d %d %d %d",
		pbs_count, scelta, q_num, test, time_calc);

	/*
		Come richiesto dalle specifiche dell'algoritmo, se la quantità
		di operandi è minore o uguale a 20, allora il valore di ogni
		singolo operando deve essere specificato dall'utente.
	*/

	if (scelta != TESTING_SUITE && q_num <= OP_MAX_QUANTITY) {
		int i = 1;
		for (i=1; i <= q_num; i++) {
			printf("Inserisci il %do operando da sommare: \n", i);
			op = getNumberFromInput();
			fprintf(pbs_file, " %f", op);
		}
	}

	fprintf(pbs_file,
				"\necho '>>>'\n"
				"echo PBS: Job completato.\n"
				"echo --- \n"
	);

	fclose(pbs_file);
}

/* **************************************************************************** */
/* RIFERIMENTI



*/
