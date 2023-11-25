#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#include "../../prova1/codice/libraries/menufunc.c"

void crea_griglia(MPI_Comm *griglia, MPI_Comm *grigliaRighe, MPI_Comm *grigliaColonne, int menum, int nproc, int riga, int col, int *coord){
    //ci manca sicuramente qualcosa - che cosa?

    int dim = 2, *ndim, reorder, *period, vc[2];
    ndim = (int *) calloc (dim, sizeof(int));
    ndim[0] = riga;
    ndim[1] = col;
    period = (int *) calloc (dim, sizeof(int));
    period[0] = period [1] = reorder = 0;

    MPI_Cart_create(MPI_COMM_WORLD, dim, ndim, period, reorder, griglia);
    //Creazione griglia
    MPI_Cart_coords(*griglia, menum, 2, coord);
    //Assegnazione coordinate dei nodi
    vc[0] = 0;
    vc[1] = 1;
    MPI_Cart_sub(*griglia, vc, grigliaRighe); //Divisione in righe del communicator
    vc[0] = 1;
    vc[1] = 0;
    MPI_Cart_sub(*griglia, vc, grigliaColonne); //Divisione in colonne del communicator
    return;
}

int MPI_Allreduce (void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm *comm){
    //... WHAT TO DO?
    return 0;
}

int MPI_Allgather (void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm *comm){
    //... WHAT TO DO?
    return 0;
}

int main (int argc, char* argv[]){
    
    int menum, nproc; // id processore "attivo" e numero processori totale
    int rows, cols; // dimensioni matrice
    int sub_rows, sub_cols; // dimensioni delle sottomatrici
    int comm_cols, comm_rows; // dimensioni della griglia di comunicazione
    int coord[2]; // vettore delle coordinate
    int i = 0, j = 0;

    float **a, **a_loc;
    float *x, *xloc, *y, *yloc;
    float *sum_loc; // vettori e matrici locali?
 
    MPI_Status status; 
    MPI_Comm *griglia, *grigliaRighe, *grigliaColonne;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &menum);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    int tag = 1;
    if (menum == 0){
        printf("Inserisci il numero di righe della matrice:\n");
        rows = getIntegerFromInput();
        printf("Inserisci il numero di colonne della matrice:\n");
        cols = getIntegerFromInput();
    }

    MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Assegnazione delle colonne e delle righe
    MPI_Bcast(&comm_cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&comm_rows, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Allocazione delle memorie necessarie
    // Creazione della griglia di processori
    crea_griglia(griglia, grigliaRighe, grigliaColonne, menum, nproc, comm_rows, comm_cols, coord);

    sub_rows = rows/comm_cols;
    sub_cols = cols/comm_rows;
    
    //Distribuzione dei dati fra i processori

    // ...

    // Calcolo del prodotto matrice-vettore
    for(i=0; i<sub_cols; i++){
        for(j=0; j<sub_rows; j++){
            yloc[i] += a_loc[i*sub_rows +j]*xloc[j]; //pare che equivalga a yloc[i] += a_loc[i][j]*xloc[j];
        }
    }

    //Comunicazione e raccolta dei risultati
    MPI_Allreduce (yloc, sum_loc, sub_cols, MPI_FLOAT, MPI_SUM, grigliaRighe); //somma sulle righe
    MPI_Allgather (sum_loc, mloc, MPI_FLOAT, y, mloc, MPI_FLOAT, grigliaColonne); //correzione sulle colonne

    // ...

    //Stampa dell'output
    if (menum == 0) {
        for (i = 0; i < cols; i++) {
            printf("y[%d] = %f", i, y[i]);
        }
    }
    
    //... WHAT TO DO?
    
    MPI_Finalize();
    return 0;
}