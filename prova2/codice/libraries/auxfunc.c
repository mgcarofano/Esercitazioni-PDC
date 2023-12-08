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
#include <tgmath.h>
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

void writeTimeCSV(int rows, int cols, int threads, int test, double t_tot) {

	FILE *csv_file;

	system(MKDIR_PATH" -p "CSV_TIME_PATH);

	if ((csv_file = fopen(CSV_TIME_PATH"/"NOME_PROVA"_time.csv", "a")) == NULL) {
		printf("Errore durante l'esecuzione!\n");
		printf("Applicazione terminata.\n");
		exit(FILE_OPENING_ERROR);
	}

	fprintf(csv_file, "%d,%d,%d,%d,%1.9f\n",
		rows, cols, threads, test, t_tot);

	fclose(csv_file);

	printf("%s aggiornato con successo!\n\n", CSV_TIME_PATH"/"NOME_PROVA"_time.csv");

}

int getRowsFromCSV(const char* path) {

    FILE* file;
	char c;
	int rows_csv = 0;

	if ((file = fopen(path, "r")) == NULL) {
		printf("Errore durante l'esecuzione!\n");
		printf("Applicazione terminata.\n");
		exit(FILE_OPENING_ERROR);
	}

	do {
		c = fgetc(file);
		if (c == 10) {
			rows_csv++;
		}
	} while (c != EOF);

	return rows_csv+1;
}

int getColsFromCSV(const char* path) {

    FILE* file;
	char c;
	int cols_csv = 0;

	if ((file = fopen(path, "r")) == NULL) {
		printf("Errore durante l'esecuzione!\n");
		printf("Applicazione terminata.\n");
		exit(FILE_OPENING_ERROR);
	}

	do {
		c = fgetc(file);
		if (c == 44) {
			cols_csv++;
		}
        if (c == 10) {
            cols_csv = 0;
        }
	} while (c != EOF);

	return cols_csv+1;
}

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

	if ((rows_csv != rows_mat) || (cols_csv != cols_mat)) {
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
			
			if (c == 10) {
				comma_count = 0;
				row_count++;
			}

			for (i = 0; i < CSV_FIELD_PRECISION; i++) {
				char_val[i] = '0';
			}
			char_val[i] ='\0';

			i = -1;
		}
	} while (c != EOF);
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

	if (rows_csv != cols_mat) {
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
			}

			for (i = 0; i < CSV_FIELD_PRECISION; i++) {
				char_val[i] = '0';
			}
			char_val[i] ='\0';

			i = -1;
		}
	} while (c != EOF);
}

void freeMatrix(double** mat, int rows_csv) {
	int i = 0;
    for (i = 0; i < rows_csv; i++) {
        free(mat[i]);
    }
    free(mat);
}

/* **************************************************************************** */
/* RIFERIMENTI



*/
