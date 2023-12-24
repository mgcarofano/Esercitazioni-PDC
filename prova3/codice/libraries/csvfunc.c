/*

	csvfunc.c
	di Mario Gabriele Carofano
	e Francesco Noviello

*/

/*	*************************************************************************** */
//	CODICE DELLE FUNZIONI DEFINITE IN 'csvfunc.h'

void getDimensionsFromCSV(FILE* csv_file, int* size, int* rows_csv, int* cols_csv) {

	char c = 0;
	int file_pointer = -1;

	*size = 0;
    *rows_csv = 0;
    *cols_csv = 0;

	fseek(csv_file, 0, SEEK_END);
	*size = ftell(csv_file);
	if (*size == 0) {
		*rows_csv = 0;
		*cols_csv = 0;
        return;
	}

	fseek(csv_file, 0, SEEK_SET);

	do {
		file_pointer++;
		c = fgetc(csv_file);

		if (c == CSV_FIELDS_SEPARATOR) {
			*cols_csv += 1;
		}

        if (c == CSV_ROWS_SEPARATOR) {
            *cols_csv = 0;
			*rows_csv += 1;
        }
	} while (file_pointer < *size);

	*cols_csv += 1;
	*rows_csv += 1;

	fseek(csv_file, 0, SEEK_SET);
}

void getMatrixFromCSV(const char* path, double* mat, int rows_mat, int cols_mat, MPI_Comm comm) {

	FILE *file;
	char c, char_val[CSV_FIELD_PRECISION] = {};
	int size = 0, rows_csv = 0, cols_csv = 0;

	int i = -1, file_pointer = -1;
	int row_count = 0, comma_count = 0;

	if ((file = fopen(path, "r")) == NULL) {
		printf("Errore durante l'esecuzione!\n");
		printf("Applicazione terminata.\n");
		MPI_Abort(comm, FILE_OPENING_ERROR);
	}

	getDimensionsFromCSV(file, &size, &rows_csv, &cols_csv);

	// printf("size: %d\n", size);
	// printf("rows_csv: %d, cols_csv: %d\n", rows_csv, cols_csv);
	// printf("rows_mat: %d, cols_mat: %d\n", rows_mat, cols_mat);

	if ((rows_mat > rows_csv) || (cols_mat > cols_csv)) {
		printf("La dimensione della matrice non e' corretta!\n");
		printf("Applicazione terminata.\n");
		MPI_Abort(comm, MATRIX_DIMENSION_ERROR);
	}

	do {
		++file_pointer;
		++i;
		c = fgetc(file);

		// printf("%c", c);
		if (c != CSV_FIELDS_SEPARATOR) {
			if (i < CSV_FIELD_PRECISION-1) {
				char_val[i] = c;
			}
		}

		/*

			Si utilizza il controllo "file_pointer == size" per evitare di
			leggere il carattere "End Of File" (EOF) che non e' riconosciuto
			correttamente dalla libreria MPI.
		
		*/

		if (file_pointer == size || c == CSV_FIELDS_SEPARATOR || c == CSV_ROWS_SEPARATOR) {
			char_val[i] = '\0';
			// printf("%s\n", char_val);

			mat[row_count*cols_mat + comma_count] = argToDouble(char_val);
			comma_count++;
			
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

	} while (file_pointer < size);

	if (fclose(file) != 0) {
		printf("Errore durante l'esecuzione!\n");
		printf("Applicazione terminata.\n");
		MPI_Abort(comm, FILE_CLOSING_ERROR);
	}

}

void writeTimeCSV(
	const char* out_path,
	int A_rows, int A_cols,
	int B_rows, int B_cols,
	int n_proc, int input, int test,
	double t_tot,
	MPI_Comm comm
) {

	FILE *csv_file;
	int size = 0;

	system(MKDIR_PATH" -p "CSV_TIME_PATH);

	if ((csv_file = fopen(out_path, "a")) == NULL) {
		printf("Errore durante l'esecuzione!\n");
		printf("Applicazione terminata.\n");
		MPI_Abort(comm, FILE_OPENING_ERROR);
	}

	// Se il file e' vuoto, allora non inserire '\n'
	fseek(csv_file, 0, SEEK_END);
    size = ftell(csv_file);
	if (size != 0) {
		fprintf(csv_file, "\n");
	}

	fseek(csv_file, 0, SEEK_SET);
	fprintf(csv_file, "%d,%d,%d,%d,%d,%d,%d,%1.9f",
		A_rows, A_cols, B_rows, B_cols, n_proc, input, test, t_tot
	);

	if (fclose(csv_file) != 0) {
		printf("Errore durante l'esecuzione!\n");
		printf("Applicazione terminata.\n");
		MPI_Abort(comm, FILE_CLOSING_ERROR);
	}

	printf("%s aggiornato con successo!\n\n", out_path);

}

/*	***************************************************************************
	RIFERIMENTI
	
	https://rookiehpc.org/mpi/docs/mpi_file_open/index.html
	https://stackoverflow.com/questions/27667758/mpi-file-read-is-not-ending
*/
