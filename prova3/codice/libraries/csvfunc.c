/*

	csvfunc.c
	di Mario Gabriele Carofano
	e Francesco Noviello

*/

/*	*************************************************************************** */
//	LIBRERIE

#include "constants.c"
#include <stdio.h>
#include <stdlib.h>
#include "auxfunc.h"

/*	*************************************************************************** */
//	CODICE DELLE FUNZIONI DEFINITE IN 'csvfunc.h'

void getDimensionsFromCSV(const char* path, int* rows_csv, int* cols_csv) {

	FILE* csv_file;
	char c = 0;

    *rows_csv = 0;
    *cols_csv = 0;

	if ((csv_file = fopen(path, "r")) == NULL) {
		printf("Errore durante l'esecuzione!\n");
		printf("Applicazione terminata.\n");
		exit(FILE_OPENING_ERROR);
	}

	fseek(csv_file, 0, SEEK_END);
	if (ftell(csv_file) == 0) {
		*rows_csv = 0;
		*cols_csv = 0;
        return;
	}

	fseek(csv_file, 0, SEEK_SET);

	do {
		c = fgetc(csv_file);
		printf("%c", c);

		if (c == EOF) {
			break;
		}

		if (c == CSV_FIELDS_SEPARATOR) {
			*cols_csv += 1;
		}

        if (c == CSV_ROWS_SEPARATOR) {
            *cols_csv = 0;
			*rows_csv += 1;
        }
	} while (1);

	*cols_csv += 1;
	*rows_csv += 1;
}

void getMatrixFromCSV(const char* path, double* mat, int rows_mat, int cols_mat) {
	FILE *file;
	char c, char_val[CSV_FIELD_PRECISION] = {};
	int rows_csv = 0, cols_csv = 0;

	int i = -1;
	int row_count = 0, comma_count = 0;

	if ((file = fopen(path, "r")) == NULL) {
		printf("Errore durante l'esecuzione!\n");
		printf("Applicazione terminata.\n");
		exit(FILE_OPENING_ERROR);
	}

	getDimensionsFromCSV(path, &rows_csv, &cols_csv);

	// printf("rows_csv: %d, cols_csv: %d\n", rows_csv, cols_csv);
	// printf("rows_mat: %d, cols_mat: %d\n", rows_mat, cols_mat);

	if ((rows_mat > rows_csv) || (cols_mat > cols_csv)) {
		printf("La dimensione della matrice non e' corretta!\n");
		printf("Applicazione terminata.\n");
		exit(MATRIX_DIMENSION_ERROR);
	}

	do {
		++i;
		c = fgetc(file);

		// printf("%c", c);
		if (c != CSV_FIELDS_SEPARATOR) {
			if (i < CSV_FIELD_PRECISION-1) {
				char_val[i] = c;
			}
		}
		
		if (c == CSV_FIELDS_SEPARATOR || c == CSV_ROWS_SEPARATOR || c == EOF) {
			char_val[i] = '\0';
			mat[row_count*cols_mat + comma_count] = argToDouble(char_val);
			comma_count++;

			if (c == EOF) {
				break;
			}
			
			if (c == CSV_ROWS_SEPARATOR || comma_count == cols_mat) {
				comma_count = 0;
				row_count++;
				if (row_count == rows_mat) {
					break;
				}
			}

			for (i = 0; i < CSV_FIELD_PRECISION; i++) {
				char_val[i] = '0';
			}
			char_val[i] ='\0';

			i = -1;
		}
	} while (1);
}

// void writeTimeCSV(const char* out_path, int rows, int cols, int threads, int test, double t_tot) {

// 	FILE *csv_file;
// 	int size = 0;

// 	system(MKDIR_PATH" -p "CSV_TIME_PATH);

// 	if ((csv_file = fopen(out_path, "a")) == NULL) {
// 		printf("Errore durante l'esecuzione!\n");
// 		printf("Applicazione terminata.\n");
// 		exit(FILE_OPENING_ERROR);
// 	}

// 	// Se il file e' vuoto, allora non inserire '\n'
// 	fseek(csv_file, 0, SEEK_END);
//     size = ftell(csv_file);
// 	if (size != 0) {
// 		fprintf(csv_file, "\n");
// 	}

// 	fseek(csv_file, 0, SEEK_SET);
// 	fprintf(csv_file, "%d,%d,%d,%d,%1.9f",
// 		rows, cols, threads, test, t_tot);

// 	fclose(csv_file);

// 	printf("%s aggiornato con successo!\n\n", out_path);

// }

// in input: file da cui leggere, matrice in cui salvare, dimensioni, processore, size processore
void read_matrix(char* filecsv, double **matrix, int *rows, int *cols, int world_rank, int world_size) {
    MPI_File file;
    FILE *file_ptr;
    MPI_Status status;
    MPI_Offset filesize, disp;
    int size, read_rows, read_cols;
    double *buf;

    if (world_rank == 0) { // il processore 0
        if ((file_ptr = fopen(filecsv, "r")) == NULL) {
            fprintf(stderr, "Unable to open the file '%s'\n", filecsv);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        fseek(file_ptr, 0, SEEK_END);
        filesize = ftell(file_ptr);
        fclose(file_ptr);

        size = sizeof(double);
        MPI_File_open(MPI_COMM_WORLD, filecsv, MPI_MODE_RDONLY, MPI_INFO_NULL, &file);
        MPI_File_read(file, &read_rows, 1, MPI_INT, &status);
        MPI_File_read(file, &read_cols, 1, MPI_INT, &status);
        *rows = read_rows;
        *cols = read_cols;

        MPI_File_close(&file);

        buf = (double *)malloc(read_rows * read_cols * size);

        MPI_Scatter(buf, read_rows * read_cols, MPI_DOUBLE, buf, read_rows * read_cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        *matrix = buf;
    } else {
        MPI_Scatter(NULL, 0, MPI_DOUBLE, *matrix, *rows * *cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }
}


/*	***************************************************************************
	RIFERIMENTI
	
*/
