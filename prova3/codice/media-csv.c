/*

	media-csv.c
	di Mario Gabriele Carofano
	e Francesco Noviello

*/

/*	*************************************************************************** */
//	COSTANTI

#define A_ROWS 0
#define A_COLS 1
#define B_ROWS 2
#define B_COLS 3
#define N_PROC 4
#define INPUT 5
#define TEST 6
#define TIME 7

#define BURST_SIZE 10

/*	*************************************************************************** */
//	LIBRERIE E ALTRE FUNZIONI

#include "libraries/csvfunc.h"

void calcola_media(double *mat, int rows, int cols, MPI_Comm comm) {

	double avg = 0.0;
	int i = 0, k = 0;

	for (k = 0; k < rows; k += BURST_SIZE) {
		avg = 0.0;

		for (i = 0; i < BURST_SIZE; i++) {
			avg += mat[(k+i)*cols + TIME];
		}

		avg /= BURST_SIZE;

		// printf("%d,%d,%d,%d,%d,%d,%d,%1.9f\n",
		// 	(int) mat[k*cols + A_ROWS],
		// 	(int) mat[k*cols + A_COLS],
		// 	(int) mat[k*cols + B_ROWS],
		// 	(int) mat[k*cols + B_COLS],
		// 	(int) mat[k*cols + N_PROC],
		// 	(int) mat[k*cols + INPUT],
		// 	(int) mat[k*cols + TEST],
		// 	avg
		// );

		writeTimeCSV(
			CSV_TIME_PATH"/"NOME_PROVA"_time_avg.csv",
			(int) mat[k*cols + A_ROWS],
			(int) mat[k*cols + A_COLS],
			(int) mat[k*cols + B_ROWS],
			(int) mat[k*cols + B_COLS],
			(int) mat[k*cols + N_PROC],
			(int) mat[k*cols + INPUT],
			(int) mat[k*cols + TEST],
			avg,
			comm
		);
	}
}

/*	*************************************************************************** */

int main(int argc, char **argv) {

	int csv_rows = 0, csv_cols = 0, csv_size = 0;
	int i = 0;
	double *mat = NULL;
	FILE *csv_mat = NULL;

	MPI_Init(&argc, &argv);

	if ((csv_mat = fopen(argv[1], "r")) == NULL) {
		printf("Nessun file o directory con questo nome: %s\n", argv[1]);
		printf("Applicazione terminata.\n");
		MPI_Abort(MPI_COMM_WORLD, FILE_OPENING_ERROR);
	}

	getDimensionsFromCSV(csv_mat, &csv_size, &csv_rows, &csv_cols);
	// printf("csv_rows: %d, csv_cols: %d\n", csv_rows, csv_cols);

	mat = (double*) calloc(csv_rows * csv_cols, sizeof(double));

	getMatrixFromCSV(csv_mat, mat, csv_rows, csv_cols, MPI_COMM_WORLD);
	calcola_media(mat, csv_rows, csv_cols, MPI_COMM_WORLD);

	if ((fclose(csv_mat)) != 0) {
		printf("Nessun file o directory con questo nome: %s\n", argv[1]);
		printf("Applicazione terminata.\n");
		MPI_Abort(MPI_COMM_WORLD, FILE_CLOSING_ERROR);
	}

	free(mat);
	MPI_Finalize();
	return 0;
}
