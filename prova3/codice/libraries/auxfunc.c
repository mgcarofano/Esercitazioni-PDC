/*

	auxfunc.c
	di Mario Gabriele Carofano
	e Francesco Noviello

*/

/*	**************************************************************************** */
//	LIBRERIE

#include "constants.c"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <time.h>

/*	**************************************************************************** */
//	CODICE DELLE FUNZIONI DEFINITE IN 'auxfunc.h'

int argToInt(char *arg) {
	
	char *p;
	long out_long = 0;

	// Si inizializza il valore di 'errno' nella libreria <errno.h>
	errno = 0;

	/*
		Si controlla che l'argomento passato al programma non sia una stringa
		vuota. La funzione 'strlen', infatti, ne misura la dimensione.
	*/

	if (strlen(arg) == 0) {
		printf("Errore nella lettura degli argomenti di input!\n\n");
		printf("Esecuzione terminata.\n");
		MPI_Finalize();
		exit(EMPTY_ARG_ERROR);
	}

	/*
		Si preferisce l'utilizzo della funzione 'strtol' invece della
		funzione 'atoi' perche' consente un maggior numero di controlli
	*/

	out_long = strtol(arg, &p, 10);

	/*
		Si controlla che l'argomento passato al programma non sia una stringa
		contenente il solo carattere '\0' (null-termination character) e non
		ci siano stati altri errori durante la sua lettura.
	*/

	if (*p != '\0' || errno != 0) {
		printf("Errore nella lettura degli argomenti di input!\n\n");
		printf("Esecuzione terminata.\n");
		MPI_Finalize();
		exit(INPUT_ARG_ERROR);
	}

	/*
		Per una corretta conversione da long a int, si controlla
		che il valore long sia rappresentabile come intero.
	*/
	
	if (out_long < INT_MIN || out_long > INT_MAX) {
		printf("Errore nella lettura degli argomenti di input!\n\n");
		printf("Esecuzione terminata.\n");
		MPI_Finalize();
		exit(NOT_INT_ARG_ERROR);
	}
	
	return (int)out_long;
}

double argToDouble(char *arg) {
	
	char *p;
	double out_double = 0.0;

	errno = 0;

	if (strlen(arg) == 0) {
		printf("Errore nella lettura degli argomenti di input!\n\n");
		printf("Esecuzione terminata.\n");
		MPI_Finalize();
		exit(EMPTY_ARG_ERROR);
	}

	out_double = strtod(arg, &p);

	if (*p != '\0' || errno != 0) {
		printf("Errore nella lettura degli argomenti di input!\n\n");
		printf("Esecuzione terminata.\n");
		MPI_Finalize();
		exit(INPUT_ARG_ERROR);
	}
	
	return out_double;
}

void getRandomMatrix(double *mat, int mat_rows, int mat_cols) {

	int i = 0, j = 0;
	double double_rand = 0.0;
	int int_rand = 0;

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
	
}

void getIdentityMatrix(double *mat, int mat_rows, int mat_cols) {

	int i = 0, j = 0;

	for (i = 0; i < mat_rows; i++) {
		for (j = 0; j < mat_cols; j++) {
			if (i == j) {
				mat[i*mat_cols + j] = 1;
			}
		}
	}

}

void getTransposeMatrix(double *A_mat, int A_rows, int A_cols, double *B_mat, int B_rows, int B_cols) {

	int i = 0, j = 0;

	if (A_rows != B_cols || A_cols != B_rows) {
		printf("Errore nella costruzione della matrice trasposta!\n\n");
		printf("Esecuzione terminata!\n");
		MPI_Finalize();
		exit(MATRIX_DIMENSION_ERROR);
	}

	for (j = 0; j < A_cols; j++) {
		for (i = 0; i < A_rows; i++) {
			B_mat[j*B_cols + i] = A_mat[i*A_cols + j];
		}
	}

}

double* scatterMatrixToGrid(
	double* mat, int mat_rows, int mat_cols,
	int* loc_rows, int* loc_cols,
	int id_grid, int* grid_dim, int* grid_coords, MPI_Comm comm_grid
) {

	int i = 0, j = 0, k = 0;
	int rest_rows = 0, rest_cols = 0;

	double* ret = NULL;

	int *send_counts = NULL, *displs = NULL;
    MPI_Datatype type_block;

	// Si distribuisce equamente il numero di righe e colonne tra tutti i processori.
	*loc_rows = mat_rows / grid_dim[0];
	*loc_cols = mat_cols / grid_dim[1];

	//	Gli operandi rimanenti sono distribuiti tra i primi 'rest' processori.
	rest_rows = mat_rows % grid_dim[0];
	if (grid_coords[0] < rest_rows) {
		*loc_rows++;
	}

	rest_cols = mat_cols % grid_dim[1];
	if (grid_coords[1] < rest_cols) {
		*loc_cols++;
	}

	// printf("%d - grid_dim[0]: %d, grid_dim[1]: %d\n", id_grid, grid_dim[0], grid_dim[1]);
	// printf("%d - loc_rows: %d, loc_cols: %d\n", id_grid, loc_rows, loc_cols);

	ret = (double*) calloc(*loc_rows * *loc_cols, sizeof(double));

	if (!ret) {
		printf("Errore nell'allocazione della matrice locale!\n");
		printf("Esecuzione terminata.\n");
		MPI_Abort(comm_grid, ALLOCATION_ERROR);
	}

	MPI_Type_vector(*loc_rows, *loc_cols, mat_cols, MPI_DOUBLE, &type_block);
    MPI_Type_create_resized(type_block, 0, *loc_rows * sizeof(double), &type_block);
    MPI_Type_commit(&type_block);

	// Il numero di elementi da distribuire a ogni processore
    send_counts = (int*) calloc(grid_dim[0] * grid_dim[1], sizeof(int));
	// int send_counts[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};

    // La posizione nella matrice "linearizzata" da dove iniziare la distribuzione
    // dei 'count' elementi corrispondenti
    displs = (int*) calloc(grid_dim[0] * grid_dim[1], sizeof(int));
	// int displs[9] = {0, 1, 2, 9, 10, 11, 18, 19, 20};

    for (int i = 0; i < grid_dim[0]; i++) {

        // printf("%d - send: ", id_grid);
        // printf("%d - displ: ", id_grid);

        for (int j = 0; j < grid_dim[1]; j++) {
            send_counts[i*grid_dim[1] + j] = 1;
            displs[i*grid_dim[1] + j] = ((i * mat_cols * *loc_rows) + (j * *loc_cols)) / *loc_cols;

			// printf("%d ", send_counts[i*grid_dim[1] + j]);
            // printf("%d ", displs[i*grid_dim[1] + j]);
        }

		// printf("\n");
    }
	// printf("\n");

	MPI_Scatterv(
        mat, send_counts, displs, type_block,
        ret, *loc_rows * *loc_cols,
        MPI_DOUBLE, 0, comm_grid
    );

	return ret;

}

void bmr_broadcast() {


	
}

void bmr_multiply(
	double* A_mat, double* B_mat, double* C_mat,
	int A_rows, int A_cols,
	int B_rows, int B_cols,
	int C_rows, int C_cols,
	MPI_Comm comm_grid
) {

	int i = 0, j = 0, k = 0;

	/*
	
		Anche se questa funzione viene invocata in un punto dell'esecuzione
		del programma nella quale le dimensioni delle matrici e della griglia
		di processori sono state gia' verificate, si preferisce comunque
		specificare i controlli sugli errori per migliorare la robustezza.

	*/

	if (B_rows != A_cols) {
		printf("Le matrici A,B non sono compatibili!\n");
		printf("Esecuzione terminata.\n");
		MPI_Abort(comm_grid, MATRIX_DIMENSION_ERROR);
	}

	if ((C_rows != A_rows) || (C_cols != B_cols)) {
		printf("Le matrici A,C non sono compatibili!\n");
		printf("Esecuzione terminata.\n");
		MPI_Abort(comm_grid, MATRIX_DIMENSION_ERROR);
	}

	for (i = 0; i < A_rows; i++) {
		for (j = 0; j < B_rows; j++) {
			for (k = 0; k < C_rows; k++) {
				C_mat[i*C_cols + j] += (A_mat[i*A_cols + k] * B_mat[k*B_cols + j]);
			}
		}
	}
	
}

void bmr_rolling() {


	
}

void bmr() {


	
}

/*	***************************************************************************
	RIFERIMENTI
	
*/
