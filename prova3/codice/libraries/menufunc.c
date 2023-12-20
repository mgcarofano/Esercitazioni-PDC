/*

	menufunc.c
	di Mario Gabriele Carofano
	e Francesco Noviello

*/

/*	*************************************************************************** */
//	LIBRERIE

#include "constants.c"

#include <stdlib.h>
#include <ctype.h>
#include <math.h>

/*	*************************************************************************** */
//	CODICE DELLE FUNZIONI DEFINITE IN 'menufunc.h'

void printTitle() {
	printf(
		"########################################################################\n"
		"#      ________      __                     __           ____________  #\n"
		"#     / ____/ /___ _/ /_  ____  _________ _/ /_____     /  _/  _/  _/  #\n"
		"#    / __/ / / __ `/ __ \\/ __ \\/ ___/ __ `/ __/ __ \\    / / / / / /    #\n"
		"#   / /___/ / /_/ / /_/ / /_/ / /  / /_/ / /_/ /_/ /  _/ /_/ /_/ /     #\n"
		"#  /_____/_/\\__,_/_.___/\\____/_/   \\__,_/\\__/\\____/  /___/___/___/     #\n"
		"#                                                                      #\n"
		"# ---- Calcolo del prodotto matrice-matrice in ambiente di calcolo --- #\n"
		"# ------- parallelo su architettura MIMD a memoria distribuita ------- #\n"
		"#                                                                      #\n"
		"########################################################################\n\n"
	);
}

double getNumberFromInput() {
	double out = 0;
	char *buffer = NULL;
	size_t bufsize = 0;
	ssize_t chars_read;

	// Inizializzazione del buffer con caratteri estratti dallo stream di input
	chars_read = getline(&buffer, &bufsize, stdin);
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

		/*
			La lettura dei caratteri inizia dall'indice chars_read-2 perche':
			-	la numerazione del buffer e' da 0 a N-1.
			-	l'ultimo carattere e' \0.
		*/

		for (i = chars_read-2; i >= 0; i--) {
			if (buffer[i] != '.' && buffer[i] != ',') {
				exp_fract--;
			} else {
				pos_point = chars_read - (exp_fract * (-1)) -2;
				break;
			}
		}

		i = chars_read-2;
		while(i >= 0) {
			if (i > pos_point && pos_point != -1) {

				if (isdigit(buffer[i])) {
					// Per ottenere il valore intero a partire dal carattere ASCII
					digit_val = pow(10, exp_fract) * (digit_val + (buffer[i] - '0'));
					out = out + digit_val;
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
					digit_val = pow(10, exp_whole) * (digit_val + (buffer[i] - '0'));
					out = out + digit_val;
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

void createPBS(
	int A_rows, int A_cols, int B_rows, int B_cols,
	int n_proc,
	int input, int test, int time_calc,
	int pbs_count)
{

	char pbs_path[PATH_MAX_LENGTH] = {};
	FILE *pbs_file;

	int i = 0, j = 0;
	int A_num = 0, B_num = 0;
	int int_op = 0;
	double double_op = 0.0;

	char *buffer = NULL;
	char A_mat_csv[PATH_MAX_LENGTH] = {};
	char B_mat_csv[PATH_MAX_LENGTH] = {};
	size_t bufsize = 0;
	ssize_t chars_read;

	if (pbs_count <= 1) {
		system(RM_PATH" -rf ../jobs");
		system(MKDIR_PATH" -p ../jobs");
	}

	sprintf(pbs_path, "../jobs/"NOME_PROVA"_%03d.pbs", pbs_count);

	if ((pbs_file = fopen(pbs_path, "a")) == NULL) {
		printf("Errore durante l'esecuzione!\n");
		printf("Applicazione terminata.\n");
		exit(FILE_OPENING_ERROR);
	}

	fprintf(pbs_file,
				"#!/bin/bash\n"
				"\n"
				"#PBS -q studenti\n"
				"#PBS -l nodes=" NODE_NUMBER
	);

	if (time_calc == OK_TIME_CALC) {
		fprintf(pbs_file, ":ppn=" NODE_PROCESS);
	}
	
	fprintf(pbs_file,
				"\n#PBS -N " NOME_PROVA "_%03d\n"
				"#PBS -o ../output/" NOME_PROVA "_%03d.out\n"
				"#PBS -e ../output/" NOME_PROVA "_%03d.err\n"
				"\n",
	pbs_count, pbs_count, pbs_count);

	if (pbs_count <= 1) {
		fprintf(pbs_file,
				"rm -fr $PBS_O_HOME/" NOME_PROVA "/output\n"
				"mkdir -p $PBS_O_HOME/" NOME_PROVA "/output\n\n"
		);
	}

	fprintf(pbs_file, "echo --- \n");

	if (time_calc == OK_TIME_CALC) {
		fprintf(pbs_file, "sort -u $PBS_NODEFILE > hostlist\n");
	}
	
	fprintf(pbs_file,
				"PBS_O_WORKDIR=$PBS_O_HOME/" NOME_PROVA "/codice\n"
				"\n"
				"echo PBS: la directory di lavoro e\\' $PBS_O_WORKDIR\n"
				"echo PBS: Compilazione in esecuzione...\n"
				COMPILER_PATH " -o $PBS_O_WORKDIR/" NOME_PROVA "_%03d -lm $PBS_O_WORKDIR/" NOME_PROVA ".c\n"
				"echo PBS: Compilazione completata.\n"
				"\n"
				"echo 'PBS: Job in esecuzione su %d cpu...'\n"
				"echo '>>>'\n",
	pbs_count, n_proc);

	if (time_calc == OK_TIME_CALC) {
		fprintf(pbs_file,
				"for i in $(seq 10)\n"
				"do\n"
				"\t" EXECUTE_PATH " -machinefile hostlist -np %d ",
		n_proc);
	} else {
		fprintf(pbs_file, EXECUTE_PATH " -machinefile $PBS_NODEFILE -n %d ", n_proc);
	}

	fprintf(pbs_file, "$PBS_O_WORKDIR/" NOME_PROVA "_%03d %d %d %d %d %d %d %d %d",
		pbs_count, A_rows, A_cols, B_rows, B_cols, input, n_proc, test, time_calc);

	/*
		Nella suite di testing progettata, se il numero totale di elementi
		delle due matrici e' minore o uguale a OP_MAX_QUANTITY, allora il 
		valore di ogni elemento deve essere specificato dall'utente
		da linea di comando.
	*/

	A_num = A_rows * A_cols;
	B_num = B_rows * B_cols;

	switch(test) {
		case DEFAULT_TEST: {
			if (input == VALUES_FROM_INPUT) {
				if ((A_num+B_num) <= OP_MAX_QUANTITY) {

					printf("Inserimento di %d elementi nella matrice A.\n", A_num);
					for (i = 1; i <= A_rows; i++) {
						for (j = 1; j <= A_cols; j++) {
							printf("Inserisci il %do elemento sulla %da riga:\n", j, i);
							double_op = getNumberFromInput();
							fprintf(pbs_file, " %f", double_op);
						}
					}

					printf("Inserimento di %d elementi nella matrice B.\n", B_num);
					for (i = 1; i <= B_rows; i++) {
						for (j = 1; j <= B_cols; j++) {
							printf("Inserisci il %do elemento sulla %da riga:\n", j, i);
							double_op = getNumberFromInput();
							fprintf(pbs_file, " %f", double_op);
						}
					}

				}
			} else if (input == VALUES_FROM_CSV) {

				printf("Inserimento di %d elementi nella matrice A.\n", A_num);
				printf("Inserisci il percorso del file .csv:\n");
				chars_read = getline(&buffer, &bufsize, stdin);
				printf("\n");

				if (chars_read < 0 || chars_read > PATH_MAX_LENGTH) {
					printf("Errore nella lettura dell'input!");
					printf("Applicazione terminata.\n");
					free(buffer);
					exit(INPUT_LINE_ERROR);
				}

				for (i = 0; i < chars_read; i++) {
					if (buffer[i] != 10) {
						A_mat_csv[i] = buffer[i];
					}
				}

				printf("Inserimento di %d elementi nella matrice B.\n", B_num);
				printf("Inserisci il percorso del file .csv:\n");
				chars_read = getline(&buffer, &bufsize, stdin);
				printf("\n");

				if (chars_read < 0 || chars_read > PATH_MAX_LENGTH) {
					printf("Errore nella lettura dell'input!");
					printf("Applicazione terminata.\n");
					free(buffer);
					exit(INPUT_LINE_ERROR);
				}

				for (i = 0; i < chars_read; i++) {
					if (buffer[i] != 10) {
						B_mat_csv[i] = buffer[i];
					}
				}

				fprintf(pbs_file, " %s %s", A_mat_csv, B_mat_csv);

				free(buffer);
			}

			break;
		}
		case MULTIPLICATION_IDENTITY_TEST:
		case MULTIPLICATION_TRANSPOSE_TEST:
		case MULTIPLICATION_TRACE_TEST:
		default:
			break;
	}

	if (time_calc == OK_TIME_CALC) {
		fprintf(pbs_file, "\ndone\n");
	} else {
		fprintf(pbs_file, "\n");
	}

	fprintf(pbs_file,
				"echo '>>>'\n"
				"echo PBS: Job completato.\n"
				"echo --- \n"
	);

	fclose(pbs_file);
	printf("%s creato con successo!\n\n", pbs_path);
}

/*	***************************************************************************
	RIFERIMENTI

*/
