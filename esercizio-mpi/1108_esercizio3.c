// https://stackoverflow.com/questions/29325513/scatter-matrix-blocks-of-different-sizes-using-mpi
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include "mpi.h"

// char** allocchar2darray(int n, int m) {
//     char **ptrs = malloc(n*sizeof(char *));
//     ptrs[0] = malloc(n*m*sizeof(char));
//     for (int i=0; i<n*m; i++)
//         ptrs[0][i]='.';

//     for (int i=1; i<n; i++) 
//         ptrs[i] = ptrs[i-1] + m;

//     return ptrs;
// }

// void freechar2darray(char **a) {
//     free(a[0]);
//     free(a);
// }

// void printarray(char **data, int n, int m) {
//     for (int i=0; i<n; i++) {
//         for (int j=0; j<m; j++) 
//             putchar(data[i][j]);
//         putchar('\n');
//     }
// }

// void twophaseRowCol(const int myrow, const int mycol, const int rank, const int size, 
//                     const int blocks[2], const int blocksize, const int globalsizes[2], const int localsizes[2],
//                     const char *const globalptr,  char **localdata) {
    
//     char **rowdata ;

//     /* create communicators which have processors with the same row or column in them*/
//     MPI_Comm colComm, rowComm;
//     MPI_Comm_split(MPI_COMM_WORLD, myrow, rank, &rowComm);
//     MPI_Comm_split(MPI_COMM_WORLD, mycol, rank, &colComm);

//     /* first, scatter the array by rows, with the processor in column 0 corresponding to each row
//      * receiving the data */
//     if (mycol == 0) {
//         int sendcounts[ blocks[0] ];
//         int senddispls[ blocks[0] ];
//         senddispls[0] = 0;

//         for (int row=0; row<blocks[0]; row++) {
//             /* each processor gets blocksize rows, each of size globalsizes[1]... */
//             sendcounts[row] = blocksize*globalsizes[1];
//             if (row > 0) 
//                 senddispls[row] = senddispls[row-1] + sendcounts[row-1];
//         }
//         /* the last processor gets one more */
//         sendcounts[blocks[0]-1] += globalsizes[1];

//         /* allocate my rowdata */
//         rowdata = allocchar2darray( sendcounts[myrow], globalsizes[1] );

//         /* perform the scatter of rows */
//         MPI_Scatterv(globalptr, sendcounts, senddispls, MPI_CHAR,
//                       &(rowdata[0][0]), sendcounts[myrow], MPI_CHAR, 0, colComm);
        
//         // MPI_Barrier(colComm);
//         // printf("\nRank %d:\n", myrow);
//         // printarray(rowdata, sendcounts[myrow], globalsizes[1]);

//     }

//     /* Now, within each row of processors, we can scatter the columns.  
//      * We can do this as we did in the previous example; create a vector
//      * (and localvector) type and scatter accordingly */
//     int locnrows = blocksize;
//     if (myrow == blocks[0]-1)
//         locnrows++;

//     MPI_Datatype vec, localvec;
//     MPI_Type_vector(locnrows, 1, globalsizes[1], MPI_CHAR, &vec);
//     MPI_Type_create_resized(vec, 0, sizeof(char), &vec);
//     MPI_Type_commit(&vec);

//     MPI_Type_vector(locnrows, 1, localsizes[1], MPI_CHAR, &localvec);
//     MPI_Type_create_resized(localvec, 0, sizeof(char), &localvec);
//     MPI_Type_commit(&localvec);

//     int sendcounts[ blocks[1] ];
//     int senddispls[ blocks[1] ];
//     if (mycol == 0) {
//         for (int col=0; col<blocks[1]; col++) {
//             sendcounts[col] = (mycol == blocks[1]-1) ? blocksize+1 : blocksize;
//             senddispls[col] = col*blocksize;
//             // printf("id_proc: %d, sendcounts[%d]: %d, senddispls[%d]: %d\n", rank, col, sendcounts[col], col, senddispls[col]);
//         }
//     }

//     char *rowptr = (mycol == 0) ? &(rowdata[0][0]) : NULL;

//     // if (mycol == 0) {
//         MPI_Scatterv(rowptr, sendcounts, senddispls, vec, &(localdata[0][0]), sendcounts[mycol], localvec, 0, rowComm);
//     // }

//     MPI_Type_free(&localvec);
//     MPI_Type_free(&vec);

//     if (mycol == 0) 
//         freechar2darray(rowdata);

//     MPI_Comm_free(&rowComm);
//     MPI_Comm_free(&colComm);

//     for (int proc=0; proc<size; proc++) {
//         if (proc == rank) {
//             printf("\nRank %d:\n", proc);
//             printarray(localdata, localsizes[0], localsizes[1]);
//         }
//         MPI_Barrier(MPI_COMM_WORLD);
//     }
// }

// int main(int argc, char **argv) {

//     int rank, size;
//     int blocks[2] = {0,0};
//     const int blocksize=3;
//     int globalsizes[2], localsizes[2];
//     char **globaldata;
//     char *globalptr = NULL;

//     MPI_Init(&argc, &argv);
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//     MPI_Comm_size(MPI_COMM_WORLD, &size);

//     /* calculate sizes for a 2d grid of processors */
//     MPI_Dims_create(size, 2, blocks);

//     int myrow, mycol;
//     myrow = rank / blocks[1];
//     mycol = rank % blocks[1];

//     /* create array sizes so that last block has 1 too many rows/cols */
//     globalsizes[0] = blocks[0]*blocksize+1;  
//     globalsizes[1] = blocks[1]*blocksize+1;
//     if (rank == 0) {
//         globaldata = allocchar2darray(globalsizes[0], globalsizes[1]);
//         globalptr = &(globaldata[0][0]);
//         for (int i=0; i<globalsizes[0]; i++) 
//             for (int j=0; j<globalsizes[1]; j++)
//                 globaldata[i][j] = 'a'+(i*globalsizes[1] + j)%26;

//         // printf("Global array: \n");
//         // printarray(globaldata, globalsizes[0], globalsizes[1]);
//     }

//     /* the local chunk we'll be receiving */
//     localsizes[0] = blocksize; localsizes[1] = blocksize;
//     if ( myrow == blocks[0]-1 ) localsizes[0]++;
//     if ( mycol == blocks[1]-1 ) localsizes[1]++;
//     char **localdata = allocchar2darray(localsizes[0],localsizes[1]);

//     twophaseRowCol(myrow, mycol, rank, size, blocks, blocksize, globalsizes, localsizes, globalptr, localdata);

//     freechar2darray(localdata);
//     if (rank == 0) 
//         freechar2darray(globaldata);

//     MPI_Finalize();

//     return 0;
// }

// -----

// https://stackoverflow.com/questions/7549316/mpi-partition-matrix-into-blocks
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define mat_rows 9
#define mat_cols 9

int main(int argc, char **argv) {

    int p, rank;

    const int grid_rows = 3;  /* number of rows in _decomposition_ */
    const int grid_cols = 3;  /* number of cols in _decomposition_ */
    const int loc_rows = mat_rows / grid_rows;  /* number of rows in _block_ */
    const int loc_cols = mat_cols / grid_cols; /* number of cols in _block_ */

    double *mat = NULL, *loc = NULL, double_rand = 0;
    int i = 0, j = 0, k = 0, int_rand = 0;

    int *send_counts = NULL, *displs = NULL;

    MPI_Datatype type_rows;
    MPI_Datatype type_cols;
    MPI_Datatype type_block;

    srand(time(NULL));

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        mat = (double*) calloc(mat_rows * mat_cols, sizeof(double*));

        for (i = 0; i < mat_rows; i++) {
            for (j = 0; j < mat_cols; j++) {
                double_rand = (double)rand();
                int_rand = (int)rand();

                // Si genera un numero casuale reale compreso tra 0 e OP_MAX_VALUE
                mat[i*mat_cols + j] = (double_rand / RAND_MAX) * 10;

                // Si ha il 33% di possibilita che mat[i][j] < 0
                if (int_rand % 3 == 0) {
                    mat[i*mat_cols + j] *= -1;
                }
            }
        }
    }

    if (p != (grid_rows * grid_cols)) {
        fprintf(stderr,"Error: number of PEs %d != %d x %d\n", p, grid_rows, grid_cols);
        MPI_Finalize();
        exit(-1);
    }
    
    loc = (double*) calloc(loc_rows * loc_cols, sizeof(double*));

    // E' un nuovo tipo di dato MPI che rappresenta una riga della matrice
    // MPI_Type_vector(mat_rows, 1, mat_rows, MPI_DOUBLE, &type_rows);
    // MPI_Type_commit(&type_rows);

    // Si utilizza un precedente tipo di dato per definirne uno nuovo
    // con parametri di lower bound ed extent diversi.
    // MPI_Type_create_resized(type_rows, 0, 1*sizeof(double), &type_cols);
    // MPI_Type_commit(&type_cols);

    MPI_Type_vector(loc_rows, loc_cols, mat_cols, MPI_DOUBLE, &type_block);
    MPI_Type_create_resized(type_block, 0, loc_rows*sizeof(double), &type_block);
    MPI_Type_commit(&type_block);

    // Il numero di elementi da distribuire a ogni processore
    send_counts = (int*) calloc(grid_rows * grid_cols, sizeof(int));

    // La posizione nella matrice "linearizzata" da dove iniziare la distribuzione
    // dei prossimi 'count' elementi
    displs = (int*) calloc(grid_rows * grid_cols, sizeof(int));

    for (int i = 0; i < grid_rows; i++) {

        // printf("%d - send: ", rank);
        // printf("%d - displ: ", rank);

        for (int j = 0; j < grid_cols; j++) {
            // send_counts[i*grid_cols + j] = loc_cols; // Caso a.
            send_counts[i*grid_cols + j] = 1; // Caso b-e.
            // displs[i*grid_cols + j] = (i * mat_cols * loc_rows) + (j * loc_cols); // Caso a-d.
            displs[i*grid_cols + j] = ((i * mat_cols * loc_rows) + (j * loc_cols)) / loc_cols; // Caso e.

            // printf("%d ", send_counts[i*grid_cols + j]);
            // printf("%d ", displs[i*grid_cols + j]);
        }

        // printf("\n");
    }
    // printf("\n");

    // a. Scatterv per distribuire elementi singoli (type = MPI_DOUBLE)
    // MPI_Scatterv(mat, send_counts, displs, MPI_DOUBLE, loc, mat_cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    // b. Scatterv per distribuire righe (type = type_rows)
    // MPI_Scatterv(mat, send_counts, displs, type_rows, loc, mat_cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    // c. Scatter per distribuire colonne (type = type_cols)
    // MPI_Scatter(mat, 1, type_cols, loc, mat_cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    // d. Scatter per distribuire blocchi con lo stesso distanziamento (type = type_block)
    // MPI_Scatter(mat, 1, type_block, loc, 9, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    // e. Scatterv per distribuire blocchi con distanziamenti custom (type = type_block)
    MPI_Scatterv(
        mat, send_counts, displs, type_block,
        loc, loc_rows * loc_cols,
        MPI_DOUBLE, 0, MPI_COMM_WORLD
    );

    /* each proc prints it's "loc" out, in order */
    for (int proc = 0; proc < p; proc++) {
        if (proc == rank) {
            printf("Rank = %d\n", rank);
            if (rank == 0) {
                printf("Global matrix: \n");
                for (i = 0; i < mat_rows; i++) {
                    for (j = 0; j < mat_cols; j++) {
                        printf("%1.2f\t", mat[i*mat_cols+j]);
                    }
                    printf("\n");
                }
            }
            printf("Local Matrix:\n");
            for (int i=0; i<loc_rows; i++) {
                for (int j=0; j<loc_cols; j++) {
                    printf("%1.2f\t", loc[i*loc_cols+j]);
                }
                printf("\n");
            }
            printf("\n");
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    if (rank == 0) {
        free(mat);
    }

    free(loc);

    MPI_Finalize();

    return 0;
}