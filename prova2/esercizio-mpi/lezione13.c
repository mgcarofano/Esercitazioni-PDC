#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

#include "../../prova1/codice/libraries/menufunc.c"

void esercizio1(int n_proc, int rows) {

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

}

void esercizio2(int id_proc, int n_proc, int q_num, double *op) {

    /* ************************************************************************ */
	// INIZIALIZZAZIONE DELL'AMBIENTE DI LAVORO

    int q_loc = 0, rest = 0, tmp = 0, tag = 0;
    int i = 0, j = 0, k = 0;
    double *op_loc;
    MPI_Status status;

    /* ************************************************************************ */

    q_loc = q_num / n_proc;

    rest = q_num % n_proc;
	if (id_proc < rest) {
		q_loc++;
	}

    op_loc = (double*)calloc (q_loc, sizeof(double));

    if (id_proc == 0) {
	
		for (i=0; i < n_proc; i++) {
			tmp = q_loc;
			if (rest != 0 && i >= rest) {
				tmp--;
			}

			if (i == 0) {
				for (j=0; j < tmp; j++) {
					op_loc[j] = op[j];
				}
			} else {
				tag = i + 25;
				MPI_Send(&op[k], tmp, MPI_DOUBLE, i, tag, MPI_COMM_WORLD);
                k = k + tmp;
			}
		}
		
	} else {
		tag = id_proc + 25;
		MPI_Recv(op_loc, q_loc, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &status);
	}

    for (j=0; j < q_loc; j++) {
        printf("Processore %d, op_loc[%d]: %f\n", id_proc, j, op_loc[j]);
    }

    free(op_loc);

}

void esercizio3() {

}

int main (int argc, char **argv) {

	/* ************************************************************************ */
	// INIZIALIZZAZIONE DELL'AMBIENTE DI LAVORO

    int exercise = 0, id_proc = 0, n_proc = 0;

    /*

        Esercizio 1.
        Siano dati P processi. Scrivere una funzione che crei una griglia di
        p x q processi, dove p x q = P, e far stampare ad ogni processo le
        proprie coordinate nella griglia (gli input devono essere P, p).
        
    */
    int rows = 0;

    /*

        Esercizio 2.
        Siano dati P processi ed un vettore V di dimensione N. Scrivere una
        funzione che distribuisca equamente (cioe, facendo in modo che il
        carico sia più bilanciato possibile) gli elementi di V tra i processi.
        
    */
    int q_num = 0, i = 0, j = 0;
    int int_rand = 0;
    double double_rand = 0.0; 
    double *op;

    /*

        Esercizio 3.
        Siano dati P processi ed una matrice A di dimensione N x M. Scrivere una
        funzione che:
        1.  Crei una griglia di dimensione p x q dei processori.
        2.  Individui p x q sottoblocchi rettangolari della matrice M.
        3.  Assegni ad ogni processore il sottoblocco di M che ha le
            coordinate corrispondenti.
        
    */

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

	if (id_proc == 0) {
		printf("\nEsecuzione terminata.\n");
	}

	MPI_Finalize();

	return 0;

}