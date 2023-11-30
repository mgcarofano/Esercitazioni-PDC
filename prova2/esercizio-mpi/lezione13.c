#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

#include "../../prova1/codice/libraries/menufunc.c"

void esercizio1(int n_proc, int rows) {

	/* ************************************************************************
		ESERCIZIO 1
		Siano dati P processi. Scrivere una funzione che crei una griglia di
		p x q processi, dove p x q = P, e far stampare ad ogni processo le
		proprie coordinate nella griglia (gli input devono essere P, p).
	*/

	/* ************************************************************************ */
	// INIZIALIZZAZIONE DELL'AMBIENTE DI LAVORO

	int id_grid = 0;
	int cols = 0;
	int dim = 2, *n_dim;
	int reorder = 0, *period, *coords;

	MPI_Comm comm_grid;

	/* ************************************************************************ */

	cols = n_proc / rows;

	coords = (int*) calloc(dim, sizeof(int));

	n_dim = (int*) calloc(dim, sizeof(int));
	n_dim[0] = rows;
	n_dim[1] = cols;

	period = (int*) calloc(dim, sizeof(int));
	period[0] = 0;
	period[1] = 0;

	/* ************************************************************************ */
	// CREAZIONE DELLA GRIGLIA BIDIMENSIONALE

	MPI_Cart_create(MPI_COMM_WORLD, dim, n_dim, period, reorder, &comm_grid);
	MPI_Comm_rank(comm_grid, &id_grid);
	MPI_Cart_coords(comm_grid, id_grid, dim, coords);

	/* ************************************************************************ */
	// STAMPA DEGLI IDENTIFICATIVI E DELLE COORDINATE DEI PROCESSORI

	printf("Sono il processore %d (%d, %d)\n", id_grid, *coords, *(coords+1));

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

	/* 	*********************************************************************** */
	//	ALLOCAZIONE DEI VETTORI LOCALI

	q_loc = q_num / n_proc;

	rest = q_num % n_proc;
	if (id_proc < rest) {
		q_loc++;
	}

	// printf("q_loc: %d, rest: %d\n\n", q_loc, rest);

	op_loc = (double*)calloc (q_loc, sizeof(double));

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

	/*
	
		MPI_Gather() - METTERE INSIEME

		L'esecuzione di questa funzione ha lo stesso risultato
		dell'esecuzione di 'n_proc' MPI_Recv da parte del processo radice, e
		l'esecuzione di una singola MPI_Send da parte di tutti i processori
		del communicator (incluso il processo radice).

		Per ricevere un vettore di dati, si può eseguire un ciclo for con un
		numero di iterazioni pari al MASSIMO numero di elementi locali tra
		tutti i processori, inviando un solo elemento per ogni iterazione.

		E' importante specificare il MASSIMO numero di elementi locali, in
		quanto il processore radice puo rimanere bloccato nell'esecuzione
		di MPI_Recv se non sono eseguite abbastanza MPI_Send.
		
		Tuttavia, questa soluzione genera un ulteriore problema: siccome il
		processore radice esegue una MPI_Recv per ogni processore del
		communicator, e' necessario che il vettore di ricezione sia abbastanza
		grande da gestire gli accessi per tutte le MPI_Recv, appunto.

		Cioe', si utilizza come dimensione del vettore di ricezione la
		seguente quantita: 'q_loc * n_proc'. Al termine dell'esecuzione,
		pero', si può iterare anche solo sui primi 'q_num' elementi, visto
		che i successivi servono solo per evitare degli eventuali
		"segmentation fault".

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

	if (id_proc == 0) {
		op_tmp = (double*)calloc (q_loc * n_proc, sizeof(double));
	}
	
	tmp = q_loc;
	if (id_proc >= rest) {
		tmp++;
	}

	for (i = 0; i < tmp; i++) {
		MPI_Gather(&op_loc[i], 1, MPI_DOUBLE, &op_tmp[n_proc * i], 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	}

	/* ************************************************************************ */
	//	STAMPA DEI VETTORI LOCALI E PULIZIA DELLA MEMORIA

	MPI_Barrier(MPI_COMM_WORLD);
	for (j=0; j < q_loc; j++) {
	    printf("Processore %d, op_loc[%d]: %f\n", id_proc, j, op_loc[j]);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	if (id_proc == 0) {
		for (j=0; j < q_num; j++) {
			printf("Processore %d, op_tmp[%d]: %f\n", id_proc, j, op_tmp[j]);
		}
	}

	free(op_loc);

	if (id_proc == 0) {
		free(op_tmp);
	}

	/*	***********************************************************************
		RIFERIMENTI

		https://www.youtube.com/watch?v=-IC4pBF4kUk
		https://www.youtube.com/watch?v=tjb2QQ9oPCE
		https://gist.github.com/ehamberg/1263868/cae1d85dee821d45fb0cd58747aaf33370f3f1ed
		https://www.open-mpi.org/doc/v1.4/man3/MPI_Gather.3.php
		https://www.open-mpi.org/doc/v1.4/man3/MPI_Scatter.3.php

	*/

}

void esercizio3() {

	/* ************************************************************************
		ESERCIZIO 3
		Siano dati P processi ed una matrice A di dimensione N x M. Scrivere una
		funzione che:
		1.  Crei una griglia di dimensione p x q dei processori.
		2.  Individui p x q sottoblocchi rettangolari della matrice M.
		3.  Assegni ad ogni processore il sottoblocco di M che ha le
			coordinate corrispondenti.
	*/

	/*	***********************************************************************
		RIFERIMENTI

		
	*/

}

int main (int argc, char **argv) {

	/* ************************************************************************ */
	// INIZIALIZZAZIONE DELL'AMBIENTE DI LAVORO

	int exercise = 0, id_proc = 0, n_proc = 0;

	int rows = 0;

	int q_num = 0, i = 0, j = 0;
	int int_rand = 0;
	double double_rand = 0.0; 
	double *op;

	// variabili esercizio 3 ...

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
				printf("Inserisci il numero di righe della griglia: \n");
				rows = getIntegerFromInput();
			}

			MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);

			esercizio1(n_proc, rows);
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
			esercizio3();
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