//dalle slides del 2021/22, sicuro non funziona...

#include <stdio.h>
#include "mpif.h"

int main (int argc, char* argv[]){
    
    int menum, nproc; //id processore "attivo" e numero processori totale
    int n,m; //dimensioni matrice
    int ncol, mcol; //dimensioni delle sottomatrici
    int nloc, mloc; //dimensioni delle sottomatrici, eventuale errore di battitura?
    int colonne, righe; //dimensioni della griglia (quale griglia?)
    int coord[2];
    int i,j;

    float *x, *A, *y; //vettori e matrice?
    float *xloc, *Aloc, *yloc, *sumblock; //vettori e matrici locali?
    /* problemi con mpi.h
    //... WHAT TO DO?   
    MPI_Status status; 
    MPI_Comm *griglia, *grigliaRighe, *grigliaColonne;
    //... WHAT TO DO?
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &menum);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    */

    int tag = 1;
    if (menum == 0){
        printf("Inserisci il numero di righe della matrice:\n"); scanf("%d", &m);
        printf("Inserisci il numero di colonne della matrice:\n"); scanf("%d", &n);
    }

    /*
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    */
    //Assegnazione delle colonne e delle righe
    /*
    MPI_Bcast(&colonne, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&righe, 1, MPI_INT, 0, MPI_COMM_WORLD);
    */

    //Allocazione delle memorie necessarie
    /*Creazione della griglia di processori*/
    /*
    crea_griglia(griglia, grigliaRighe, grigliaColonne, menum, nproc, righe, colonne, coord)
    */

    //... WHAT TO DO?
    mloc = m/righe;
    nloc = n/colonne;
    
    //Distribuzione dei dati fra i processori

    //Calcolo
    for(i=0; i<mloc; i++){
        for(j=0; j<nloc; j++){
            yloc[i] += Aloc[i*nloc +j]*xloc[j]; //pare che equivalga a yloc[i] += Aloc[i][j]*xloc[j];
        }
    }

    //Comunicazione e raccolta dei risultati
    /*
    MPI_Allreduce (yloc, sumblock, mloc, MPI_FLOAT, MPI_SUM, grigliaRighe); //somma sulle righe
    MPI_Allgather (sumblock, mloc, MPI_FLOAT, y, mloc, MPI_FLOAT, grigliaColonne); //correzione sulle colonne
    */

    //... WHAT TO DO?

    //Stampa dell'output
    if(menum == 0){
        for(i=0; i<m; i++){
            printf("y[%d] = %f", i, y[i]);
        }
    }
    
    //... WHAT TO DO?
    
    /*
    MPI_Finalize();
    */
    return 0;
}

/*
void crea_griglia( MPI_Comm *griglia, MPI_Comm *grigliaRighe, MPI_Comm *grigliaColonne, int menum, int nproc, int riga, int col, int *coord){
    //ci manca sicuramente qualcosa

    int dim, *ndim, reorder, *period, vc[2];
    int dim = 2;
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

int MPI_Allreduce (void sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm){
    //... WHAT TO DO?
    return 0;
}

int MPI_Allgather (void sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm){
    //... WHAT TO DO?
    return 0;
}
*/