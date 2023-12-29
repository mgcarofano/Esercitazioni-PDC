/*

	auxfunc.c
	di Mario Gabriele Carofano
	e Francesco Noviello

*/

/*	**************************************************************************** */
//	LIBRERIE

#include "constants.c"
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
		printf("La matrice locale non e' stata allocata correttamente!\n");
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

    for (i = 0; i < grid_dim[0]; i++) {

        // printf("%d - send: ", id_grid);
        // printf("%d - displ: ", id_grid);

        for (j = 0; j < grid_dim[1]; j++) {
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

	free(send_counts);
	free(displs);

	return ret;

}

void fprintfMatrix(FILE* out_file, double* mat, int rows, int cols, const char* format) {

	int i = 0, j = 0;
	int mode = TAB_SPACE_PRINT;

	switch(mode) {
		case TAB_SPACE_PRINT:
		{
			for (i = 0; i < rows; i++) {
				for (j = 0; j < cols; j++) {
					fprintf(out_file, format, mat[i*cols + j]);
					if (j != cols-1) {
						fprintf(out_file, "\t");
					}
				}
				if (i != rows-1) {
					fprintf(out_file, "\n");
				}
			}
			fprintf(out_file, "\n");
			break;
		}
		case WOLFRAM_PRINT:
		{
			fprintf(out_file, "{");
			for (i = 0; i < rows; i++) {
				fprintf(out_file, "{");
				for (j = 0; j < cols; j++) {
					fprintf(out_file, format, mat[i*cols + j]);
					if (j != cols-1) {
						fprintf(out_file, ", ");
					}
				}
				fprintf(out_file, "}");
				if (i != rows-1) {
					fprintf(out_file, ", ");
				}
			}
			fprintf(out_file, "}");
			fprintf(out_file, "\n");
			break;
		}
		default:
			break;
	}

}

void bmr_broadcast(
	double* mat, double* tmp,
	int rows, int cols,
	int step,
	MPI_Comm comm_grid, MPI_Comm comm_row
) {

	int grid_rank = 0, grid_coord[2] = {};
	int row_size = 0, row_rank = 0;
	int dest = 0;
	int k = 0;

	MPI_Status status;

	MPI_Comm_rank(comm_grid, &grid_rank);
	MPI_Cart_coords(comm_grid, grid_rank, 2, grid_coord);

	// fprintf(out_file, "%d %d", grid_coord[0], grid_coord[1]);

	MPI_Comm_size(comm_row, &row_size);
	MPI_Comm_rank(comm_row, &row_rank);

	if ((grid_coord[0]+step) % row_size == grid_coord[1]) {
		// fprintf(out_file, "SEND - row_rank: %d\n", row_rank);
		for (k = 0; k < row_size; k++) {
			dest = (row_rank + k + 1) % row_size;
			// fprintf(out_file, "dest: %d\n", dest);
			if (dest == row_rank) {
				// fprintf(out_file, "COPY\n");
				memcpy(tmp, mat, rows * cols * sizeof(double));
			} else {
				// fprintf(out_file, "SEND\n");
				MPI_Send(&(*mat), rows * cols, MPI_DOUBLE, dest, step * BROADCAST_TAG, comm_row);
			}
		}
	} else {
		// fprintf(out_file, "RECV - row_rank: %d\n", row_rank);
		MPI_Recv(&(*tmp), rows * cols, MPI_DOUBLE, MPI_ANY_SOURCE, step * BROADCAST_TAG, comm_row, &status);
	}
	
}

void bmr_multiply(
	double* A_mat, double* B_mat, double* C_mat,
	int A_rows, int A_cols,
	int B_rows, int B_cols,
	int C_rows, int C_cols,
	MPI_Comm comm
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
		MPI_Abort(comm, MATRIX_DIMENSION_ERROR);
	}

	if ((C_rows != A_rows) || (C_cols != B_cols)) {
		printf("Le matrici A,C non sono compatibili!\n");
		printf("Esecuzione terminata.\n");
		MPI_Abort(comm, MATRIX_DIMENSION_ERROR);
	}

	for (i = 0; i < A_rows; i++) {
		for (j = 0; j < B_rows; j++) {
			for (k = 0; k < C_rows; k++) {
				C_mat[i*C_cols + j] += (A_mat[i*A_cols + k] * B_mat[k*B_cols + j]);
			}
		}
	}
	
}

void bmr_rolling(
	double* mat, double* tmp,
	int rows, int cols,
	int step,
	MPI_Comm comm_col
) {

	// int id_proc = 0;
	int col_size = 0, col_rank = 0;
	int dest = 0, source = 0;
	MPI_Status status;

	// MPI_Comm_rank(comm, &id_proc);
	MPI_Comm_size(comm_col, &col_size);
	MPI_Comm_rank(comm_col, &col_rank);

	/*
	
		Si calcolano gli identificativi dei processori nel comunicatore
		di colonna alla quale inviare (dest) e dalla quale ricevere (source)
		il blocco di matrice per la fase di rolling.

	*/

	source = (col_rank + step) % col_size;
	dest = (col_rank - step) % col_size;

	/*
	
		A questo punto si dividono due casi:
		-	Se il modulo calcolato per l'identificativo 'dest' e' negativo,
			allora il processore chiamante si trova ai confini della griglia.
			In questo caso, si effettua prima l'operazione di MPI_Recv e poi
			l'operazione di MPI_Send.
		-	In caso opposto, si effettua prima l'operazione di MPI_Send e poi
			l'operazione di MPI_Recv.
		
		E' stata implementata questa politica di comunicazione per evitare
		un eventuale deadlock, cioe' una situazione in cui tutti i processori
		invocano contemporaneamente una chiamata a MPI_Send o MPI_Recv.
	
	*/

	if (dest < 0) {
		// Si ricalcola l'identificativo 'dest' perche' non puo' essere negativo.
		dest = (dest + col_size) % col_size;
		MPI_Recv(&(*tmp), rows * cols, MPI_DOUBLE, source, ROLLING_TAG, comm_col, &status);
		MPI_Send(&(*mat), rows * cols, MPI_DOUBLE, dest, ROLLING_TAG, comm_col);
	} else {
		MPI_Send(&(*mat), rows * cols, MPI_DOUBLE, dest, ROLLING_TAG, comm_col);
		MPI_Recv(&(*tmp), rows * cols, MPI_DOUBLE, source, ROLLING_TAG, comm_col, &status);
	}

	// if (dest < 0) {
	// 	fprintf(out_file, "\t%d (%d) RECV from %d\n", col_rank, id_proc, source);
	// 	fprintf(out_file, "\t%d (%d) SEND to %d\n", col_rank, id_proc, dest);
	// } else {
	// 	fprintf(out_file, "\t%d (%d) SEND to %d\n", col_rank, id_proc, dest);
	// 	fprintf(out_file, "\t%d (%d) RECV from %d\n", col_rank, id_proc, source);
	// }

}

void bmr(
	double* A_mat, double* B_mat, double* C_mat,
	int A_rows, int A_cols,
	int B_rows, int B_cols,
	int C_rows, int C_cols,
	MPI_Comm comm, MPI_Comm comm_row, MPI_Comm comm_col 
) {

	int n_proc = 0, id_proc = 0, col_size = 0;
	int step = 0;

	double *tmp_A_mat = NULL, *tmp_B_mat = NULL;

	MPI_Comm_size(comm, &n_proc);
	MPI_Comm_rank(comm, &id_proc);

	if (n_proc == 1) {

		bmr_multiply(
			A_mat, B_mat, C_mat,
			A_rows, A_cols,
			B_rows, B_cols,
			C_rows, C_cols,
			comm
		);

	} else {

		MPI_Comm_size(comm_col, &col_size);
		// if (id_proc == 0 && DEBUG) printf("%d\n", col_size);

		tmp_A_mat = (double*) calloc(A_rows * A_cols, sizeof(double));
		tmp_B_mat = (double*) calloc(B_rows * B_cols, sizeof(double));

		if (!tmp_A_mat && !tmp_B_mat) {
			printf("Le matrici locali non sono state allocate correttamente!\n");
			printf("Esecuzione terminata.\n");
			MPI_Abort(comm, ALLOCATION_ERROR);
		}

		for (step = 0; step < col_size; step++) {

			if (id_proc == 0 && DEBUG) printf("\n--- PASSO %d ---\n\n", step+1);
			// fprintf(out_file, "\n--- PASSO %d ---\n", step+1);

			bmr_broadcast(
				A_mat, tmp_A_mat,
				A_rows, A_cols,
				step,
				comm, comm_row
			);

			if (id_proc == 0 && DEBUG) printf("\tBROADCAST\n");
			// fprintf(out_file, "\nBROADCAST");
			// fprintf(out_file, "\nMatrice A di dimensione %d x %d:\n", A_rows, A_cols);
			// fprintfMatrix(out_file, A_mat, A_rows, A_cols, "%f\t");
			// fprintf(out_file, "\nMatrice tmp_A di dimensione %d x %d:\n", A_rows, A_cols);
			// fprintfMatrix(out_file, tmp_A_mat, A_rows, A_cols, "%f\t");

			if (step == 0) {

				bmr_multiply(
					tmp_A_mat, B_mat, C_mat,
					A_rows, A_cols,
					B_rows, B_cols,
					C_rows, C_cols,
					comm
				);

			} else {
				bmr_rolling(
					B_mat, tmp_B_mat,
					B_rows, B_cols,
					step,
					comm_col
				);

				if (id_proc == 0 && DEBUG) printf("\tROLLING\n");
				// fprintf(out_file, "\nROLLING");
				// fprintf(out_file, "\nMatrice B di dimensione %d x %d:\n", B_rows, B_cols);
				// fprintfMatrix(out_file, B_mat, B_rows, B_cols, "%f\t");
				// fprintf(out_file, "\nMatrice tmp_B di dimensione %d x %d:\n", B_rows, B_cols);
				// fprintfMatrix(out_file, tmp_B_mat, B_rows, B_cols, "%f\t");

				bmr_multiply(
					tmp_A_mat, tmp_B_mat, C_mat,
					A_rows, A_cols,
					B_rows, B_cols,
					C_rows, C_cols,
					comm
				);
			}

			if (id_proc == 0 && DEBUG) printf("\tMULTIPLY\n");
			// fprintf(out_file, "\nMULTIPLY");
			// fprintf(out_file, "\nMatrice C di dimensione %d x %d:\n", C_rows, C_cols);
			// fprintfMatrix(out_file, C_mat, C_rows, C_cols, "%f\t");

			if (id_proc == 0 && DEBUG) printf("\n-----\n\n");
			// fprintf(out_file, "\n-----\n\n");
		}

		free(tmp_A_mat);
		free(tmp_B_mat);
	}
}

/*	***************************************************************************
	RIFERIMENTI

	https://www.cs.csi.cuny.edu/~gu/teaching/courses/csc76010/slides/Matrix%20Multiplication%20by%20Nur.pdf
	https://www.geeksforgeeks.org/modulus-on-negative-numbers/
	https://stackoverflow.com/questions/12923039/mpi-send-receive-programme-never-completes
	
*/
