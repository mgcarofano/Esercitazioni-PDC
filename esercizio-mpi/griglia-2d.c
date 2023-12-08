#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#include "../../prova1/codice/libraries/menufunc.c"

int main (int argc, char **argv) {

	/* ************************************************************************ */
	// INIZIALIZZAZIONE DELL'AMBIENTE DI LAVORO

	int id_proc = 0, n_proc = 0, id_grid = 0;
	int rows = 0, cols = 0;
	int dim = 2, *n_dim;
	int reorder = 0, *period, *coords;

	MPI_Comm comm_grid;

	/* ************************************************************************ */
	// INIZIALIZZAZIONE DEI PROCESSORI

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id_proc);
	MPI_Comm_size(MPI_COMM_WORLD, &n_proc);

	if (id_proc == 0) {
		printf("Inserisci il numero di righe della griglia: \n");
		rows = getIntegerFromInput();
	}

	MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);

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

	/*
		int MPI_Cart_create(MPI_Comm comm_old, int ndims, int *dims,
			int *periods, int reorder, MPI_Comm *comm_cart)

		MPI_Cart_create è un'operazione collettiva che restituisce un
		riferimento ad un nuovo 'communicator' al quale assegnare le
		informazioni della topologia a griglia bidimensionale (Cartesian
		topology).

		Se 'reorder = false', allora il rank di ogni processore del nuovo
		gruppo è identico al rank del gruppo precedente. Altrimenti, tale
		funzione ha la capacita di riordinare i processori (in modo da
		fornire la miglior topologia virtuale rispetto alla macchina fisica).

		Se la dimensione totale della griglia bidimensionale è minore della
		dimensione del gruppo del communicator, allora per alcuni processori
		si restituisce il valore MPI_COMM_NULL. Invece, se la dimensione è
		maggiore, allora una chiamata a questa funzione restituisce un errore.

		comm_old:	il communicator in input.
		ndims:		numero di dimensioni della griglia (in questo caso 2).
		dims:		numero di processori per ogni dimensione della griglia.
		periods:	array di valori true / false per specificare se la
					dimensione deve essere periodica o meno, rispettivamente.
		reorder:	valore true / false per specificare se è possibile il
					riordinamento dei rank o meno, rispettivamente.
		comm_cart:	riferimento al communicator di output associato
					alla griglia.

	*/

	MPI_Cart_create(MPI_COMM_WORLD, dim, n_dim, period, reorder, &comm_grid);

	/*
	
		Dopo aver definito la topologia a griglia bidimensionale, è
		necessario specificare i nuovi rank e l'associazione con le
		corrispondenti coordinate della griglia utilizzando le
		seguenti funzioni:
		•	int MPI_Comm_rank(MPI_Comm comm, int *rank)
		•	int MPI_Cart_coords(MPI_Comm comm, int rank, int maxdims,
				int *coords)

	*/

	MPI_Comm_rank(comm_grid, &id_grid);
	MPI_Cart_coords(comm_grid, id_proc, dim, coords);

	/* ************************************************************************ */
	// TERMINAZIONE DELL'ESECUZIONE

	printf("Sono il processore %d (%d, %d)\n", id_proc, *coords, *(coords+1));

	MPI_Finalize();
	return 0;

}