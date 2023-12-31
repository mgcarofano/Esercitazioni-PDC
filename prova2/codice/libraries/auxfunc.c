/*

	auxfunc.c
	di Mario Gabriele Carofano
	e Francesco Noviello

*/

/* **************************************************************************** */
// LIBRERIE

#include "constants.c"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

/* **************************************************************************** */
// CODICE DELLE FUNZIONI DEFINITE IN 'auxfunc.h'

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
		exit(INPUT_ARG_ERROR);
	}

	/*
		Per una corretta conversione da long a int, si controlla
		che il valore long sia rappresentabile come intero.
	*/
	
	if (out_long < INT_MIN || out_long > INT_MAX) {
		printf("Errore nella lettura degli argomenti di input!\n\n");
		printf("Esecuzione terminata.\n");
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
		exit(EMPTY_ARG_ERROR);
	}

	out_double = strtod(arg, &p);

	if (*p != '\0' || errno != 0) {
		printf("Errore nella lettura degli argomenti di input!\n\n");
		printf("Esecuzione terminata.\n");
		exit(INPUT_ARG_ERROR);
	}
	
	return out_double;
}

void writeTimeCSV(const char* out_path, int rows, int cols, int threads, int test, double t_tot) {

	FILE *csv_file;
	int size = 0;

	system(MKDIR_PATH" -p "CSV_TIME_PATH);

	if ((csv_file = fopen(out_path, "a")) == NULL) {
		printf("Errore durante l'esecuzione!\n");
		printf("Applicazione terminata.\n");
		exit(FILE_OPENING_ERROR);
	}

	// Se il file e' vuoto, allora non inserire '\n'
	fseek(csv_file, 0, SEEK_END);
    size = ftell(csv_file);
	if (size != 0) {
		fprintf(csv_file, "\n");
	}

	fseek(csv_file, 0, SEEK_SET);
	fprintf(csv_file, "%d,%d,%d,%d,%1.9f",
		rows, cols, threads, test, t_tot);

	fclose(csv_file);

	printf("%s aggiornato con successo!\n\n", out_path);

}

void freeMatrix(double** mat, int rows) {
	int i = 0;
    for (i = 0; i < rows; i++) {
        free(mat[i]);
    }
    free(mat);
}

int getRowsFromCSV(const char* path) {

    FILE* csv_file;
	char c = 0;
	int rows_csv = 0, size = 0;

	if ((csv_file = fopen(path, "r")) == NULL) {
		printf("Errore durante l'esecuzione!\n");
		printf("Applicazione terminata.\n");
		exit(FILE_OPENING_ERROR);
	}

	fseek(csv_file, 0, SEEK_END);
    size = ftell(csv_file);
	if (size == 0) {
		return 0;
	}

	fseek(csv_file, 0, SEEK_SET);

	do {
		c = fgetc(csv_file);
		if (c == 10) {
			rows_csv++;
		}
	} while (c != EOF);

	return rows_csv+1;
}

int getColsFromCSV(const char* path) {

    FILE* csv_file;
	char c = 0;
	int cols_csv = 0, size = 0;

	if ((csv_file = fopen(path, "r")) == NULL) {
		printf("Errore durante l'esecuzione!\n");
		printf("Applicazione terminata.\n");
		exit(FILE_OPENING_ERROR);
	}

	fseek(csv_file, 0, SEEK_END);
    size = ftell(csv_file);
	if (size == 0) {
		return 0;
	}

	fseek(csv_file, 0, SEEK_SET);

	do {
		c = fgetc(csv_file);
		if (c == 44) {
			cols_csv++;
		}
        if (c == 10) {
            cols_csv = 0;
        }
	} while (c != EOF);

	return cols_csv+1;
}

// void getCSVDimensions(const char* path, int* rows_csv, int* cols_csv) {

// 	FILE* csv_file;
// 	char c = 0;
// 	int size = 0;

// 	if ((csv_file = fopen(path, "r")) == NULL) {
// 		printf("Errore durante l'esecuzione!\n");
// 		printf("Applicazione terminata.\n");
// 		exit(FILE_OPENING_ERROR);
// 	}

// 	fseek(csv_file, 0, SEEK_END);
//     size = ftell(csv_file);
// 	if (size == 0) {
// 		*rows_csv = 0;
// 		*cols_csv = 0;
// 	}

// 	fseek(csv_file, 0, SEEK_SET);

// 	do {
// 		c = fgetc(csv_file);
// 		if (c == CSV_FIELDS_SEPARATOR) {
// 			*cols_csv++;
// 		}
//         if (c == CSV_ROWS_SEPARATOR) {
//             *cols_csv = 0;
// 			*rows_csv++;
//         }
// 	} while (c != EOF);

// 	*cols_csv += 1;
// 	*rows_csv += 1;

// }

void getMatrixFromCSV(const char* path, double** mat, int rows_mat, int cols_mat) {
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

	rows_csv = getRowsFromCSV(path);
	cols_csv = getColsFromCSV(path);

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
		if (c != 44) {
			if (i < CSV_FIELD_PRECISION) {
				char_val[i] = c;
			} else {
				printf("Errore nella lettura dell'input!\n\n");
				printf("Applicazione terminata.\n");
				exit(OVERFLOW_ERROR);
			}
		}
		
		if (c == 44 || c == 10 || c == EOF) {
			char_val[i] = '\0';
			mat[row_count][comma_count++] = argToDouble(char_val);

			if (c == EOF) {
				break;
			}
			
			if (c == 10 || comma_count == cols_mat) {
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

void getVectorFromCSV(const char* path, double* vet, int cols_mat) {
	FILE *file;
	char c, char_val[CSV_FIELD_PRECISION] = {};
	int rows_csv = 0;

	int i = -1;
	int row_count = 0;

	if ((file = fopen(path, "r")) == NULL) {
		printf("Errore durante l'esecuzione!\n");
		printf("Applicazione terminata.\n");
		exit(FILE_OPENING_ERROR);
	}

	rows_csv = getRowsFromCSV(path);

	// printf("rows_csv: %d, cols_mat: %d\n", rows_csv, cols_mat);

	if (cols_mat > rows_csv) {
		printf("La dimensione del vettore non e' corretta!\n");
		printf("Applicazione terminata.\n");
		exit(VECTOR_DIMENSION_ERROR);
	}

	do {
		++i;
		c = fgetc(file);

		// printf("%c", c);
		if (c != 10) {
			if (i < CSV_FIELD_PRECISION) {
				char_val[i] = c;
			} else {
				printf("Errore nella lettura dell'input!\n\n");
				printf("Applicazione terminata.\n");
				exit(OVERFLOW_ERROR);
			}
		}
		
		if (c == 10 || c == EOF) {
			char_val[i] = '\0';
			vet[row_count] = argToDouble(char_val);

			if (c == EOF) {
				break;
			}
			
			if (c == 10) {
				row_count++;
				if (row_count == cols_mat) {
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

// void getVectorFromCSV(const char* path, double* vet, int cols_mat) {

// 	double** mat;
// 	int i = 0;

// 	mat = (double**) calloc(cols_mat, sizeof(double*));
// 	for (i = 0; i < cols_mat; i++) {
// 		mat[i] = (double*) calloc(1, sizeof(double));
// 	}

// 	getMatrixFromCSV(path, mat, cols_mat, 1);

// 	for (i = 0; i < cols_mat; i++) {
// 		mat[i][0] = vet[i];
// 	}

// 	freeMatrix(mat, cols_mat);
// }

/* **************************************************************************** */
/* RIFERIMENTI

	https://stackoverflow.com/questions/13566082/how-to-check-if-a-file-has-content-or-not-using-c
	https://bl831.als.lbl.gov/~gmeigs/scripting_help/printf_awk_notes.txt
	https://www.tutorialspoint.com/how-can-we-return-multiple-values-from-a-function-in-c-cplusplus
	
*/
