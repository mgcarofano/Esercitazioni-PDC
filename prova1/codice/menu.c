/* **************************************************************************** */
// LIBRERIE

#include "./libraries/auxfunc.h"

/* **************************************************************************** */

int main(int argc, char **argv) {

	/* ************************************************************************ */
	// DEFINIZIONE DELLE VARIABILI

	int scelta = 0, q_num = 0, time_calc = 0;
	int count = 0;

	double op = 0.0;

	FILE *pbs_file, *qsub_out, *out_file, *err_file;

	char char_to_read, *wait_input;

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
			exit(NOT_ENOUGH_OPERANDS);
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
			exit(FILE_OPENING_ERROR);
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
			exit(FILE_OPENING_ERROR);
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

		fprintf(pbs_file, "$PBS_O_WORKDIR/" NOME_PROVA " %d %d %d", scelta, q_num, time_calc);

		if (scelta != 4 && q_num <= 20) {
			int i = 1;
			for (i=1; i <= q_num; i++) {
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
			exit(FILE_OPENING_ERROR);
		}

		if ((out_file = fopen("../output/"NOME_PROVA".out", "r")) == NULL) {
			printf("Errore nella lettura dell'output!\n\n");
			printf("Applicazione terminata.\n");
			exit(FILE_OPENING_ERROR);
		}

		fseek(err_file, 0, SEEK_END); // seek to end of file
		err_size = ftell(err_file); // get current file pointer
		fseek(err_file, 0, SEEK_SET); // seek back to beginning of file

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
		printf("\n");

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
