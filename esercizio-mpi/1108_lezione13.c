#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

#include "../prova2/codice/libraries/menufunc.c"

void esercizio1(int n_proc, int grid_rows) {

	/* ************************************************************************
		ESERCIZIO 1
		Siano dati P processi. Scrivere una funzione che crei una griglia di
		p x q processi, dove p x q = P, e far stampare ad ogni processo le
		proprie coordinate nella griglia (gli input devono essere P, p).
	*/

	/* ************************************************************************ */
	// INIZIALIZZAZIONE DELL'AMBIENTE DI LAVORO

	int id_grid = 0;
	int grid_cols = 0;
	int dim = 2, *n_dim;
	int reorder = 0, *period, *coords;

	MPI_Comm comm_grid;

	/* ************************************************************************ */
	// CREAZIONE DELLA GRIGLIA BIDIMENSIONALE

	grid_cols = n_proc / grid_rows;

	coords = (int*) calloc(dim, sizeof(int));

	n_dim = (int*) calloc(dim, sizeof(int));
	n_dim[0] = grid_rows;
	n_dim[1] = grid_cols;

	period = (int*) calloc(dim, sizeof(int));
	period[0] = 0;
	period[1] = 0;

	MPI_Cart_create(MPI_COMM_WORLD, dim, n_dim, period, reorder, &comm_grid);
	MPI_Comm_rank(comm_grid, &id_grid);
	MPI_Cart_coords(comm_grid, id_grid, dim, coords);

	/* ************************************************************************ */
	// STAMPA DEGLI IDENTIFICATIVI E DELLE COORDINATE DEI PROCESSORI

	printf("Sono il processore %d (%d, %d)\n", id_grid, coords[0], coords[1]);
	
	/* ************************************************************************ */

	free(coords);
	free(n_dim);
	free(period);

	/*	***********************************************************************
		RIFERIMENTI

		https://wgropp.cs.illinois.edu/courses/cs598-s15/lectures/lecture28.pdf
	*/

}

void esercizio2(int id_proc, int n_proc, int q_num, double *op) {

	/* ************************************************************************
		ESERCIZIO 2
		Siano dati P processi ed un vettore V di dimensione N. Scrivere una
		funzione che distribuisca equamente (cioe, facendo in modo che il
		carico sia più bilanciato possibile) gli elementi di V tra i processi.
	*/

	/* ************************************************************************ */
	// INIZIALIZZAZIONE DELL'AMBIENTE DI LAVORO

	int q_loc = 0, rest = 0, tmp = 0, tag = 0;
	int i = 0, j = 0;
	double *op_loc, *op_tmp;
	MPI_Status status;

	FILE *out_file;
	char out_path[PATH_MAX_LENGTH] = {};

	/* 	*********************************************************************** */

	// system(MKDIR_PATH" -p /output");
	sprintf(out_path, "./esercizio-mpi/output/esercizio2/proc_%02d.out", id_proc);

	if ((out_file = fopen(out_path, "w")) == NULL) {
		printf("Errore durante l'esecuzione!\n");
		printf("Applicazione terminata.\n");
		exit(FILE_OPENING_ERROR);
	}

	/* 	*********************************************************************** */
	//	ALLOCAZIONE DEI VETTORI LOCALI

	q_loc = q_num / n_proc;

	rest = q_num % n_proc;
	if (id_proc < rest) {
		q_loc++;
	}

	// printf("q_loc: %d, rest: %d\n\n", q_loc, rest);

	op_loc = (double*)calloc (q_loc, sizeof(double));

	/*	*********************************************************************** */
	//	ALTERNATIVA: DISTRIBUZIONE DEGLI OPERANDI CON MPI_SCATTER

	/*
	
		MPI_Scatter() - DIFFONDERE

		L'esecuzione di questa funzione ha lo stesso risultato
		dell'esecuzione di 'n_proc' MPI_Send da parte del processo radice, e
		l'esecuzione di una singola MPI_Recv da parte di tutti i processori
		del communicator (incluso il processo radice).

		Per inviare un vettore di dati, si può eseguire un ciclo for con un
		numero di iterazioni pari al numero di elementi locali del vettore di
		ricezione, inviando un solo elemento per ogni iterazione.

		-	void *sendbuf:
			Indirizzo del vettore da dividere tra i processori (importante
			solo per il processore root).

		-	int sendcount:
			Quantita di elementi da inviare ad ogni processore (importante
			solo per il processore root).

		-	MPI_Datatype sendtype:
			Tipo dei dati da inviare (importante solo per il processore root).

		-	void *recvbuf:
			Indirizzo del vettore locale del processore nel quale ricevere
			i dati.

		-	int recvcount:
			Quantita di elementi da ricevere.

		-	MPI_Datatype recvtype:
			Tipo dei dati da ricevere.

		-	int root:
			Identificativo del processo radice.

		-	MPI_Comm comm:
			Communicator del gruppo di processori.

	*/

	for (i = 0; i < q_loc; i++) {
		MPI_Scatter(&op[n_proc * i], 1, MPI_DOUBLE, &op_loc[i], 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	}

	/*	*********************************************************************** */
	//	STAMPA DEI VETTORI LOCALI E PULIZIA DELLA MEMORIA

	MPI_Barrier(MPI_COMM_WORLD);

	// fprintf(out_file, "Indirizzo del vettore: %p\n", &op);
	fprintf(out_file, "id_grid: %d, q_loc: %d\n", id_proc, q_loc);

	if (id_proc == 0) {
		fprintf(out_file, "Vettore globale di dimensione %d:\n", q_num);
		for (i = 0; i < q_num; i++) {
			fprintf(out_file, "%1.2f\n", op[i]);
		}
		fprintf(out_file, "\n");
	}

	fprintf(out_file, "Vettore locale di dimensione %d:\n", q_loc);
	for (i = 0; i < q_loc; i++) {
		fprintf(out_file, "%1.2f\n", op_loc[i]);
	}

	fclose(out_file);
	free(op_loc);

	/* 	*********************************************************************** */
	//	DISTRIBUZIONE DEGLI OPERANDI CON MPI_SEND e MPI_RECV

	// if (id_proc == 0) {
	
	// 	tmp = q_loc;

	// 	// printf("id_proc: %d, tmp: %d, j: %d\n", i, tmp, j);

	// 	for (j=0; j < tmp; j++) {
	// 		op_loc[j] = op[j];
	// 	}

	// 	for (i=1; i < n_proc; i++) {

	// 		if (rest != 0 && i == rest) {
	// 			tmp--;
	// 		}

	// 		tag = i + 25;
	// 		MPI_Send(&op[j], tmp, MPI_DOUBLE, i, tag, MPI_COMM_WORLD);

	// 		j += tmp;
	// 		// printf("id_proc: %d, tmp: %d, j: %d, tag: %d\n, ", i, tmp, j, tag);

	// 	}

	// 	// printf("\n");
		
	// } else {
	// 	tag = id_proc + 25;
	// 	// printf("id_proc: %d, tag: %d\n", id_proc, tag);
	// 	MPI_Recv(op_loc, q_loc, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &status);
	// }

	/* ************************************************************************ */

	/*
	
		MPI_Gather() - METTERE INSIEME

		L'esecuzione di questa funzione ha lo stesso risultato
		dell'esecuzione di 'n_proc' MPI_Recv da parte del processo radice, e
		l'esecuzione di una singola MPI_Send da parte di tutti i processori
		del communicator (incluso il processo radice).

		Per ricevere un vettore di dati, si può eseguire un ciclo for con un
		numero di iterazioni pari al MASSIMO numero di elementi locali tra
		tutti i processori, inviando un solo elemento per ogni iterazione.

		Si sceglie il MASSIMO numero di elementi locali in
		quanto il processore radice puo rimanere bloccato nell'esecuzione
		di MPI_Recv, se non sono eseguite abbastanza MPI_Send. A questo
		proposito si propongono due soluzioni:

		1.	Si utilizza come dimensione del vettore di ricezione la quantita
			'q_loc * n_proc'. Il processore radice, in questo modo, e'
			abbastanza grande da gestire gli accessi per tutte le MPI_Recv. Al
			termine dell'esecuzione, si può iterare anche solo sui primi 'q_num'
			elementi (i successivi servono solo per evitare degli eventuali
			segmentation fault).
		
		2.	Sapendo che una chiamata a MPI_Gather() è il risultato di 'n_proc'
			chiamate a MPI_Send() da parte del processore radice, allora si
			eseguono un numero di chiamate a MPI_Gather() in modo da ottenere
			tante MPI_Send() quanto il primo numero divisibile per 'n_proc'
			precedente a 'q_num'. Il resto del vettore si puo' distribuire
			utilizzando MPI_Send() e MPI_Recv().

		-	void *sendbuf:
			Indirizzo del vettore da cui inviare gli elementi.

		-	int sendcount:
			Quantita di elementi da inviare al processore radice.

		-	MPI_Datatype sendtype:
			Tipo dei dati da inviare.

		-	void *recvbuf:
			Indirizzo del vettore di ricezione del processore radice nel quale
			ricevere i dati (importante solo per il processore root).

		-	int recvcount:
			Quantita di elementi da ricevere da ogni processore (importante
			solo per il processore root).

		-	MPI_Datatype recvtype:
			Tipo dei dati da ricevere (importante solo per il processore root).

		-	int root:
			Identificativo del processo radice, cioe' quello dedicato alla
			ricezione e al "gather" dei dati.

		-	MPI_Comm comm:
			Communicator del gruppo di processori.
			
	*/

	// if (id_proc == 0) {
	// 	// op_tmp = (double*)calloc (q_loc * n_proc, sizeof(double));
	// 	op_tmp = (double*)calloc (q_num, sizeof(double));
	// }
	
	// tmp = q_loc;
	// if (id_proc >= rest) {
	// 	tmp++;
	// }

	// tmp = q_loc;
	// if (id_proc < rest) {
	// 	tmp--;
	// }

	// for (i = 0; i < tmp; i++) {
	// 	MPI_Gather(&op_loc[i], 1, MPI_DOUBLE, &op_tmp[n_proc * i], 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	// }

	// // MPI_Barrier(MPI_COMM_WORLD);
	// if (id_proc >= rest) {
	// 	MPI_Send(&op_loc[i], 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	// }

	// i *= n_proc;
	// if (id_proc == 0) {
	// 	for (j = rest; j < n_proc; j++) {
	// 		printf("j: %d, rest: %d, n_proc: %d\n", j, rest, n_proc);
	// 		MPI_Recv(&op_tmp[i], 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
	// 		i++;
	// 	}
	// }

	// MPI_Barrier(MPI_COMM_WORLD);
	// if (id_proc == 0) {
	// 	for (j=0; j < q_num; j++) {
	// 		printf("Processore %d, op_tmp[%d]: %f\n", id_proc, j, op_tmp[j]);
	// 	}
	// }

	// if (id_proc == 0) {
	// 	free(op_tmp);
	// }

	/*	***********************************************************************
		RIFERIMENTI

		https://www.youtube.com/watch?v=-IC4pBF4kUk
		https://www.youtube.com/watch?v=tjb2QQ9oPCE
		https://gist.github.com/ehamberg/1263868/cae1d85dee821d45fb0cd58747aaf33370f3f1ed
		https://www.open-mpi.org/doc/v1.4/man3/MPI_Gather.3.php
		https://www.open-mpi.org/doc/v1.4/man3/MPI_Scatter.3.php

	*/

}

void esercizio3(const int n_proc, int grid_rows, double* mat, int mat_rows, int mat_cols) {

	/* ************************************************************************
		ESERCIZIO 3
		Siano dati P processi ed una matrice A di dimensione N x M. Scrivere una
		funzione che:
		1.  Crei una griglia di dimensione p x q dei processori.
		2.  Individui p x q sottoblocchi rettangolari della matrice M.
		3.  Assegni ad ogni processore il sottoblocco di M che ha le
			coordinate corrispondenti.
	*/

	/* ************************************************************************ */
	// INIZIALIZZAZIONE DELL'AMBIENTE DI LAVORO

	int id_grid = 0;
	int grid_cols = 0;
	int dim = 2, *n_dim = NULL;
	int reorder = 0, *period = NULL, *coords = NULL;

	MPI_Comm comm_grid;

	int i = 0, j = 0, k = 0;
	int loc_rows = 0, loc_cols = 0;
	int tmp_rows = 0, tmp_cols = 0, rest_rows = 0, rest_cols = 0;

	int *send_counts = NULL, *displs = NULL;
    MPI_Datatype type_block;

	double *mat_loc = NULL;

	FILE *out_file;
	char out_path[PATH_MAX_LENGTH] = {};

	/*	*********************************************************************** */
	//	1) CREAZIONE DELLA GRIGLIA BIDIMENSIONALE

	grid_cols = n_proc / grid_rows;

	if (n_proc != (grid_rows * grid_cols)) {
        fprintf(stderr,
			"Attenzione! "
			"I processori non possono essere inseriti "
			"in una griglia bidimensionale uniforme.\n"
		);
        MPI_Finalize();
        exit(-1);
    }

	coords = (int*) calloc(dim, sizeof(int));

	n_dim = (int*) calloc(dim, sizeof(int));
	n_dim[0] = grid_rows;
	n_dim[1] = grid_cols;

	period = (int*) calloc(dim, sizeof(int));
	period[0] = 0;
	period[1] = 0;

	MPI_Cart_create(MPI_COMM_WORLD, dim, n_dim, period, reorder, &comm_grid);
	MPI_Comm_rank(comm_grid, &id_grid);
	MPI_Cart_coords(comm_grid, id_grid, dim, coords);

	/*	*********************************************************************** */

	// system(MKDIR_PATH" -p /output");
	sprintf(out_path, "./esercizio-mpi/output/esercizio3/proc%d_%02d_%02d.out", id_grid, coords[0], coords[1]);

	if ((out_file = fopen(out_path, "w")) == NULL) {
		printf("Errore durante l'esecuzione!\n");
		printf("Applicazione terminata.\n");
		exit(FILE_OPENING_ERROR);
	}

	/*	*********************************************************************** */
	//	2) DIVISIONE DELLA MATRICE IN SOTTOBLOCCHI

	// Si distribuisce equamente il numero di righe e colonne tra tutti i processori.
	loc_rows = mat_rows / n_dim[0];
	loc_cols = mat_cols / n_dim[1];

	//	Gli operandi rimanenti sono distribuiti tra i primi 'rest' processori.
	rest_rows = mat_rows % n_dim[0];
	if (coords[0] < rest_rows) {
		loc_rows++;
	}

	rest_cols = mat_cols % n_dim[1];
	if (coords[1] < rest_cols) {
		loc_cols++;
	}

	mat_loc = (double*) calloc(loc_rows * loc_cols, sizeof(double));

	/*	*********************************************************************** */
	//	3) ASSEGNAZIONE DEI SOTTOBLOCCHI AI PROCESSORI DELLA GRIGLIA

	MPI_Type_vector(loc_rows, loc_cols, mat_cols, MPI_DOUBLE, &type_block);
    MPI_Type_create_resized(type_block, 0, loc_rows*sizeof(double), &type_block);
    MPI_Type_commit(&type_block);

	// Il numero di elementi da distribuire a ogni processore
    send_counts = (int*) calloc(grid_rows * grid_cols, sizeof(int*));

    // La posizione nella matrice "linearizzata" da dove iniziare la distribuzione
    // dei 'count' elementi corrispondenti
    displs = (int*) calloc(grid_rows * grid_cols, sizeof(int*));

    for (int i = 0; i < grid_rows; i++) {
        for (int j = 0; j < grid_cols; j++) {
            send_counts[i*grid_cols + j] = 1;
            displs[i*grid_cols + j] = ((i * mat_cols * loc_rows) + (j * loc_cols)) / loc_cols;
        }
    }

	MPI_Scatterv(
        mat, send_counts, displs, type_block,
        mat_loc, loc_rows * loc_cols,
        MPI_DOUBLE, 0, comm_grid
    );

	MPI_Barrier(comm_grid);
	// if (id_grid == 0) fprintf(out_file, "Indirizzo della matrice: %p\n", &(**mat));
	// if (id_grid == 0) fprintf(out_file, "Primo elemento della matrice: %f\n", **mat);
	fprintf(out_file, "id_grid: %d, loc_rows: %d, loc_cols: %d\n",
		id_grid, loc_rows, loc_cols
	);

	if (id_grid == 0) {
		fprintf(out_file, "Matrice globale di dimensione %d x %d:\n", mat_rows, mat_cols);
		for (i = 0; i < mat_rows; i++) {
			for (j = 0; j < mat_cols; j++) {
				fprintf(out_file, "%1.2f\t", mat[i*mat_cols + j]);
			}
			fprintf(out_file, "\n");
		}
		fprintf(out_file, "\n");
	}

	fprintf(out_file, "Matrice locale di dimensione %d x %d:\n", loc_rows, loc_cols);
	for (i = 0; i < loc_rows; i++) {
		for (j = 0; j < loc_cols; j++) {
			fprintf(out_file, "%1.2f\t", mat_loc[i*loc_cols + j]);
		}
		fprintf(out_file, "\n");
	}

	fclose(out_file);

	free(mat_loc);

	/*	***********************************************************************
		RIFERIMENTI

		https://www.open-mpi.org/doc/v1.4/man3/MPI_Type_vector.3.php
		https://www.open-mpi.org/doc/v1.4/man3/MPI_Type_create_resized.3.php
		https://www.open-mpi.org/doc/v1.4/man3/MPI_Scatterv.3.php
		https://stackoverflow.com/questions/7549316/mpi-partition-matrix-into-blocks
		https://engineering.purdue.edu/~smidkiff/ece563/slides/MPI2.pdf

	*/

}

int main (int argc, char **argv) {

	/* ************************************************************************ */
	// INIZIALIZZAZIONE DELL'AMBIENTE DI LAVORO

	int exercise = 0, id_proc = 0, n_proc = 0;

	// ESERCIZIO 1
	int grid_rows = 0;

	// ESERCIZIO 2
	int q_num = 0, i = 0, j = 0;
	int int_rand = 0;
	double double_rand = 0.0; 
	double *op = NULL;

	// ESERCIZIO 3
	double *mat = NULL;
	int mat_rows = 0, mat_cols = 0;

	srand(time(NULL));

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id_proc);
	MPI_Comm_size(MPI_COMM_WORLD, &n_proc);

	/* ************************************************************************ */
	// SVOLGIMENTO DEGLI ESERCIZI

	if (id_proc == 0) {
		printf("Scegli un'operazione da effettuare: \n");
		printf("1. \t Svolgere l'esercizio 1.\n");
		printf("2. \t Svolgere l'esercizio 2.\n");
		printf("3. \t Svolgere l'esercizio 3.\n");
		printf("4. \t Chiudere l'applicazione.\n\n");
		exercise = getIntegerFromInput();
		checkScelta(exercise, 1, 4);
	}

	MPI_Bcast(&exercise, 1, MPI_INT, 0, MPI_COMM_WORLD);

	switch (exercise) {
		case 1:
		{
			if (id_proc == 0) {
				printf("Inserisci il numero di righe della griglia di processori: \n");
				grid_rows = getIntegerFromInput();
			}

			MPI_Bcast(&grid_rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
			esercizio1(n_proc, grid_rows);

			break;
		}
		case 2:
		{
			if (id_proc == 0) {
				printf("Inserisci la quantita' di numeri da distribuire: \n");
				q_num = getIntegerFromInput();

				op = (double*) calloc(q_num, sizeof(double));

				for (j=0; j < q_num; j++) {
					double_rand = (double)rand();
					int_rand = (int)rand();

					op[j] = (double_rand / RAND_MAX) * OP_MAX_VALUE;

					if (int_rand % 3 == 0) {
						op[j] = op[j] * (-1);
					}

					// printf("op[%d]: %f\n", j, op[j]);
				}

				// printf("Indirizzo del vettore: %p\n", &op);
			}

			MPI_Bcast(&q_num, 1, MPI_INT, 0, MPI_COMM_WORLD);
			esercizio2(id_proc, n_proc, q_num, op);

			if (id_proc == 0) {
				free(op);
			}
			
			break;
		}
		case 3:
		{

			/*
			
				Attenzione!
				
				Funziona per matrici quadrate e matrici rettangolari per cui
				il numero di righe e colonne è multiplo del numero di processori
				sulle righe / colonne della griglia.

				Funziona solo per griglie di processori bidimensionali quadrate.

			*/

			if (id_proc == 0) {

				printf("Inserisci la quantita' di righe della matrice:\n");
				mat_rows = getIntegerFromInput();

				if (mat_rows < 1) {
					printf("Devi inserire almeno una riga!\n");
					printf("Applicazione terminata.\n");
					exit(MATRIX_DIMENSION_ERROR);
				}

				printf("Inserisci la quantita' di colonne della matrice:\n");
				mat_cols = getIntegerFromInput();

				if (mat_cols < 1) {
					printf("Devi inserire almeno una colonna!\n");
					printf("Applicazione terminata.\n");
					exit(MATRIX_DIMENSION_ERROR);
				}

				printf("Inserisci il numero di righe della griglia di processori: \n");
				grid_rows = getIntegerFromInput();

				if (grid_rows < 1) {
					printf("Devi inserire almeno una riga di processori!\n");
					printf("Applicazione terminata.\n");
					exit(MATRIX_DIMENSION_ERROR);
				}

				mat = (double*) calloc(mat_rows * mat_cols, sizeof(double));

				for (i = 0; i < mat_rows; i++) {
					for (j = 0; j < mat_cols; j++) {
						double_rand = (double)rand();
						int_rand = (int)rand();

						// Si genera un numero casuale reale compreso tra 0 e OP_MAX_VALUE
						mat[i*mat_cols + j] = (double_rand / RAND_MAX) * OP_MAX_VALUE;

						// Si ha il 33% di possibilita che mat[i*mat_cols + j] < 0
						if (int_rand % 3 == 0) {
							mat[i*mat_cols + j] *= -1;
						}
					}
				}

				// printf("Indirizzo della matrice: %p\n", &(**mat));
				// printf("Primo elemento della matrice: %f\n", **mat);

			}

			MPI_Bcast(&grid_rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
			MPI_Bcast(&mat_rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
			MPI_Bcast(&mat_cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
			esercizio3(n_proc, grid_rows, mat, mat_rows, mat_cols);

			if (id_proc == 0) {
				free(mat);
			}

			break;
		}
		case 4:
		default:
			break;
	}

	/* ************************************************************************ */
	// TERMINAZIONE DELL'ESECUZIONE

	MPI_Barrier(MPI_COMM_WORLD);
	if (id_proc == 0) {
		printf("\nEsecuzione terminata.\n");
	}

	MPI_Finalize();

	return 0;

}