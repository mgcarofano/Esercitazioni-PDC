/*

	media-csv.c
	di Mario Gabriele Carofano
	e Francesco Noviello

*/

/*	*************************************************************************** */
//	COSTANTI

#define ROWS_FIELD 0
#define COLS_FIELD 1
#define THREADS_FIELD 2
#define TEST_FIELD 3
#define TIME_FIELD 4

#define BURST_SIZE 10

/*	*************************************************************************** */
//	LIBRERIE E ALTRE FUNZIONI

#include "./libraries/auxfunc.h"

void calcola_media(double **mat, int rows) {

	double avg = 0.0;
	int i = 0, k = 0;

	for (k = 0; k < rows; k += BURST_SIZE) {
		avg = 0.0;

		for (i = 0; i < BURST_SIZE; i++) {
			avg += mat[k+i][TIME_FIELD];
		}

		avg /= BURST_SIZE;

		// printf("%1.0f,%1.0f,%1.0f,%1.0f,%1.9f\n",
		// 	mat[k][ROWS_FIELD],
		// 	mat[k][COLS_FIELD],
		// 	mat[k][THREADS_FIELD],
		// 	mat[k][TEST_FIELD],
		// 	avg
		// );

		writeTimeCSV(
			CSV_TIME_PATH"/"NOME_PROVA"_time_avg.csv",
			(int) mat[k][ROWS_FIELD],
			(int) mat[k][COLS_FIELD],
			(int) mat[k][THREADS_FIELD],
			(int) mat[k][TEST_FIELD],
			avg
		);
	}
}

/*	*************************************************************************** */

int main(int argc, char **argv) {

	int rows = 0, cols = 0, i = 0;
	char *csv_path = NULL;
	double **mat = NULL;

	csv_path = argv[1];
	rows = getRowsFromCSV(csv_path);
	cols = getColsFromCSV(csv_path);

	mat = (double**) calloc(rows, sizeof(double*));
	for (i = 0; i < rows; i++) {
		mat[i] = (double*) calloc(cols, sizeof(double));
	}

	// printf("rows: %d, cols: %d\n", rows, cols);

	getMatrixFromCSV(csv_path, mat, rows, cols);
	calcola_media(mat, rows);
	freeMatrix(mat, rows);

	return 0;

}
