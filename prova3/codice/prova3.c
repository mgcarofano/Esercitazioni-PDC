/*

	prova3.c
	di Mario Gabriele Carofano
	e Francesco Noviello

*/

//	LIBRERIE
/*	************************************************************************ */

#include "libraries/auxfunc.h"
#include "libraries/csvfunc.h"

/*	************************************************************************ */

int main(int argc, char **argv) {

	//	INIZIALIZZAZIONE DELL'AMBIENTE DI LAVORO
	/*	******************************************************************** */

	int input = DEFAULT_INPUT, test = DEFAULT_TEST, time_calc = NO_TIME_CALC;
	int pbs_count = 0;

	int i = 0, j = 0, k = 0;

	double *A_mat = NULL, *B_mat = NULL;
	int A_rows = 0, A_cols = 0, B_rows = 0, B_cols = 0;
	int q_num = 0;

	double *loc_A_mat = NULL, *loc_B_mat = NULL, *loc_C_mat = NULL;
	int loc_A_rows = 0, loc_A_cols = 0;
	int loc_B_rows = 0, loc_B_cols = 0;
	int loc_C_rows = 0, loc_C_cols = 0;

	double t_start = 0.0, t_end = 0.0;
	double t_loc = 0.0, t_tot = 0.0;

	double grid_tmp = 0;
	int n_proc = 0, id_proc = 0, id_grid = 0;
	int grid_cols = 0, grid_rows = 0;
	int *grid_dim = NULL, *period = NULL, *grid_coords = NULL;

	MPI_Comm comm_grid;

	FILE* out_file;
	char out_path[PATH_MAX_LENGTH] = {};

	srand(time(NULL));

	//	INIZIALIZZAZIONE DELL'AMBIENTE MPI
	/*	*********************************************************************** */

	/*
		--- int MPI_Init(int *argc, char ***argv) ---
		Definisce l'insieme dei processori attivati (contesto),
		assegnandone un identificativo.
	*/

	MPI_Init(&argc, &argv);
	
	/*
		--- int MPI_Comm_rank(MPI_Comm comm, int *rank) ---
		Assegna ad ogni processore del communicator l'identificativo
		'id_proc' (sempre associato al contesto).
		MPI_COMM_WORLD indica il communicator a cui appartengono tutti
		i processori attivati (non puo' essere alterato).
	*/

	MPI_Comm_rank(MPI_COMM_WORLD, &id_proc);
	
	/*
		--- int MPI_Comm_size(MPI_Comm comm, int *size) ---
		Restituisce ad ogni processore di MPI_COMM_WORLD
		il numero di processori nel contesto.
	*/

	MPI_Comm_size(MPI_COMM_WORLD, &n_proc);

	//	LETTURA DEI DATI
	/*	******************************************************************** */

	if (id_proc == 0) {
		printf("Inizio esecuzione.\n\n");

		/*
			Si affida al primo processore con id_proc == 0 il compito
			di leggere gli argomenti in input 'argv[]'.
		*/

		if (argc < ARGS_QUANTITY) {
			printf("Errore nella lettura degli argomenti di input!\n\n");
			printf("Esecuzione terminata.\n");
			MPI_Abort(MPI_COMM_WORLD, NOT_ENOUGH_ARGS_ERROR);
		}

		/*
			--- int argToInt(char *arg) ---
			Si utilizza la funzione 'argToInt' definita in 'auxfunc.h'
			per leggere gli argomenti contenuti nel vettore di 
			stringhe 'argv[]' e convertirli in valori interi.
		*/

		A_rows = argToInt(argv[1]);
		A_cols = argToInt(argv[2]);
		B_rows = argToInt(argv[3]);
		B_cols = argToInt(argv[4]);

		if (B_rows != A_cols) {
			printf("Le matrici A,B non sono compatibili!\n");
			printf("Esecuzione terminata.\n");
			MPI_Abort(MPI_COMM_WORLD, MATRIX_DIMENSION_ERROR);
		}

		grid_tmp = sqrt(n_proc);

		if (n_proc != 0 && grid_tmp != floor(grid_tmp)) {
			printf("Il numero di processori (%d) non e' un quadrato perfetto.\n", n_proc);
			printf("Impossibile costruire una griglia di processori bidimensionale quadrata.\n");
			printf("Esecuzione terminata.\n");
			MPI_Abort(MPI_COMM_WORLD, PROCESSOR_QUANTITY_ERROR);
		}

		grid_rows = grid_cols = floor(grid_tmp);

		if ((A_cols % grid_cols != 0) || (B_cols % grid_cols != 0)) {
			printf("Le matrici non sono compatibili con la griglia di processori!\n");
			printf("Esecuzione terminata.\n");
			MPI_Abort(MPI_COMM_WORLD, MATRIX_DIMENSION_ERROR);
		}

		input = argToInt(argv[5]);
		test = argToInt(argv[6]);
		time_calc = argToInt(argv[7]);

		pbs_count = argToInt(argv[8]);

	}

	MPI_Bcast(&A_rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&A_cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&B_rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&B_cols, 1, MPI_INT, 0, MPI_COMM_WORLD);

	MPI_Bcast(&grid_rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&grid_cols, 1, MPI_INT, 0, MPI_COMM_WORLD);

	MPI_Bcast(&input, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&test, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&time_calc, 1, MPI_INT, 0, MPI_COMM_WORLD);

	MPI_Bcast(&pbs_count, 1, MPI_INT, 0, MPI_COMM_WORLD);

	//	CREAZIONE DELLA GRIGLIA BIDIMENSIONALE
	/*	******************************************************************** */

	grid_coords = (int*) calloc(2, sizeof(int));

	grid_dim = (int*) calloc(2, sizeof(int));
	grid_dim[0] = grid_rows;
	grid_dim[1] = grid_cols;

	period = (int*) calloc(2, sizeof(int));
	period[0] = 1;
	period[1] = 1;

	MPI_Cart_create(MPI_COMM_WORLD, 2, grid_dim, period, 0, &comm_grid);
	MPI_Comm_rank(comm_grid, &id_grid);
	MPI_Cart_coords(comm_grid, id_grid, 2, grid_coords);

	//	LETTURA E/O GENERAZIONE DEGLI ELEMENTI DELLA MATRICE
	/*	******************************************************************** */

	if (id_grid == 0) {

		A_mat = (double*) calloc(A_rows * A_cols, sizeof(double));
		B_mat = (double*) calloc(B_rows * B_cols, sizeof(double));

		if (!A_mat && !B_mat) {
			printf("Le matrici non sono state allocate correttamente!\n");
			printf("Esecuzione terminata.\n");
			MPI_Abort(comm_grid, ALLOCATION_ERROR);
		}

		q_num = (A_rows * A_cols) + (B_rows * B_cols);

		switch (test) {
			case DEFAULT_TEST:
			{
				switch (input) {
					case VALUES_FROM_INPUT:
					{
						if (q_num <= OP_MAX_QUANTITY) {
							k = 1;
							for (i = 0; i < A_rows; i++) {
								for (j = 0; j < A_cols; j++) {
									A_mat[i*A_cols + j] = argToDouble(argv[ARGS_QUANTITY+k]);
									k++;
								}
							}

							for (i = 0; i < B_rows; i++) {
								for (j = 0; j < B_cols; j++) {
									B_mat[i*B_cols + j] = argToDouble(argv[ARGS_QUANTITY+k]);
									k++;
								}
							}
						} else {
							getRandomMatrix(A_mat, A_rows, A_cols);
							getRandomMatrix(B_mat, B_rows, B_cols);
						}
						break;
					}
					case VALUES_FROM_CSV:
					{
						getMatrixFromCSV(argv[ARGS_QUANTITY+1], A_mat, A_rows, A_cols, comm_grid);
						getMatrixFromCSV(argv[ARGS_QUANTITY+2], B_mat, B_rows, B_cols, comm_grid);
						break;
					}
					default:
						break;
				}
				break;
			}
			case MULTIPLICATION_IDENTITY_TEST:
			{
				getRandomMatrix(A_mat, A_rows, A_cols);
				getIdentityMatrix(B_mat, B_rows, B_cols);
				break;
			}
			case MULTIPLICATION_TRANSPOSE_TEST:
			{
				getRandomMatrix(A_mat, A_rows, A_cols);
				getTransposeMatrix(A_mat, A_rows, A_cols, B_mat, B_rows, B_cols);
				break;
			}
			case MULTIPLICATION_TRACE_TEST:
			{
				getRandomMatrix(A_mat, A_rows, A_cols);
				getRandomMatrix(B_mat, B_rows, B_cols);
				break;
			}
			default:
				break;
		}
	}

	//	DISTRIBUZIONE DELLE MATRICI
	/*	******************************************************************** */

	sprintf(out_path, "../output/" NOME_PROVA "_%03d/proc%d_%02d_%02d.out",
		pbs_count, id_grid, grid_coords[0], grid_coords[1]
	);

	if ((out_file = fopen(out_path, "w")) == NULL) {
		printf("Errore durante l'esecuzione!\n");
		printf("Esecuzione terminata.\n");
		MPI_Abort(comm_grid, FILE_OPENING_ERROR);
	}

	loc_A_mat = scatterMatrixToGrid(
		A_mat, A_rows, A_cols,
		&loc_A_rows, &loc_A_cols,
		id_grid, grid_dim, grid_coords, comm_grid
	);

	loc_B_mat = scatterMatrixToGrid(
		B_mat, B_rows, B_cols,
		&loc_B_rows, &loc_B_cols,
		id_grid, grid_dim, grid_coords, comm_grid
	);

	fprintf(out_file, "Matrice A di dimensione %d x %d:\n", loc_A_rows, loc_A_cols);
	for (i = 0; i < loc_A_rows; i++) {
		for (j = 0; j < loc_A_cols; j++) {
			fprintf(out_file, "%f\t", loc_A_mat[i*loc_A_cols + j]);
		}
		fprintf(out_file, "\n");
	}
	fprintf(out_file, "\n");

	fprintf(out_file, "Matrice B di dimensione %d x %d:\n", loc_B_rows, loc_B_cols);
	if (test == MULTIPLICATION_IDENTITY_TEST) {
		for (i = 0; i < loc_B_rows; i++) {
			for (j = 0; j < loc_B_cols; j++) {
				fprintf(out_file, "%1.0f\t", loc_B_mat[i*loc_B_cols + j]);
			}
			fprintf(out_file, "\n");
		}
		fprintf(out_file, "\n");
	} else {
		for (i = 0; i < loc_B_rows; i++) {
			for (j = 0; j < loc_B_cols; j++) {
				fprintf(out_file, "%f\t", loc_B_mat[i*loc_B_cols + j]);
			}
			fprintf(out_file, "\n");
		}
		fprintf(out_file, "\n");
	}

	/*
	
		Non appena termina con successo la distribuzione delle matrici
		"A_mat" e "B_mat" dal processore radice a tutti gli altri processori
		della griglia, si puo' liberare la memoria allocata.

	*/

	if (id_grid == 0) {
		free(A_mat);
		free(B_mat);
	}
	
	// 	INIZIO DEL CALCOLO DEI TEMPI DI ESECUZIONE
	/*	******************************************************************** */

	if (time_calc == OK_TIME_CALC) {

		/*
			--- int MPI_Barrier(MPI_Comm comm) ---
			Si utilizza questa funzione per restituire il controllo al
			chiamante solo dopo che tutti i processori del contesto 'comm'
			hanno effettuato la chiamata.
			Per calcolare correttamente i tempi di esecuzione in
			sicurezza, si aspetta che tutti i processori siano sincronizzati.
		*/

		MPI_Barrier(comm_grid);

		/*
			--- double MPI_Wtime() ---
			Si utilizza per ottenere un valore di tempo in secondi
			rispetto ad un tempo arbitrario nel passato.
		*/

		t_start = MPI_Wtime();
	}

	//	APPLICAZIONE DEL BROADCAST MULTIPLY ROLLING PER IL CALCOLO DEL PRODOTTO MATRICE-MATRICE
	/*	******************************************************************** */

	loc_C_rows = loc_A_rows;
	loc_C_cols = loc_B_cols;
	loc_C_mat = (double*) calloc(loc_C_rows * loc_C_cols, sizeof(double));

	// if (n_proc == 1) {

	// } else {
		
	// }

	bmr_multiply(
		loc_A_mat, loc_B_mat, loc_C_mat,
		loc_A_rows, loc_A_cols,
		loc_B_rows, loc_B_cols,
		loc_C_rows, loc_C_cols,
		comm_grid
	);

	//	SALVATAGGIO DEL CALCOLO DEI TEMPI DI ESECUZIONE
	/*	******************************************************************** */

	if (time_calc == OK_TIME_CALC) {
		t_end = MPI_Wtime();

		// Si calcola la distanza di tempo tra l'istante iniziale e quello finale.
		t_loc = t_end - t_start;

		/*
			--- int MPI_Reduce(void *sendbuf, void *recvbuf, int count,
    				MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm) ---
			Si utilizza questa funzione per eseguire un'operazione collettiva
			su tutti i processori del contesto. In questo caso, si desidera 
			calcolare il massimo tempo tra tutti i tempi calcolati localmente.
		*/

		MPI_Reduce(&t_loc, &t_tot, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

		if (id_grid == 0) {
			printf("\nCalcolo del prodotto matrice-matrice terminato in %e sec\n", t_tot);
			writeTimeCSV(
				CSV_TIME_PATH"/"NOME_PROVA"_time.csv",
				A_rows, A_cols,
				B_rows, B_cols,
				n_proc, input, test,
				t_tot,
				comm_grid
			);
		}
	}

	//	STAMPA DELL'OUTPUT
  	/*	******************************************************************** */

	fprintf(out_file, "Risultato:\n");
	fprintf(out_file, "Matrice C di dimensione %d x %d:\n", loc_C_rows, loc_C_cols);
	for (i = 0; i < loc_C_rows; i++) {
		for (j = 0; j < loc_C_cols; j++) {
			fprintf(out_file, "%f\t", loc_C_mat[i*loc_C_cols + j]);
		}
		fprintf(out_file, "\n");
	}
	fprintf(out_file, "\n");

	//	TERMINAZIONE DELL'ESECUZIONE
  	/*	******************************************************************** */

	/*
		Attendiamo che tutti i processori abbiano portato a termine
		correttamente il loro carico di lavoro.
	*/

	MPI_Barrier(comm_grid);

	// Al termine dell'esecuzione, si libera lo spazio allocato in memoria.
	free(loc_A_mat);
	free(loc_B_mat);
	free(loc_C_mat);

	if (id_grid == 0) {

		if (fclose(out_file) != 0) {
			printf("Errore durante l'esecuzione!\n");
			printf("Esecuzione terminata.\n");
			MPI_Abort(comm_grid, FILE_CLOSING_ERROR);
		}

		printf("\nEsecuzione terminata.\n");
	}

	MPI_Finalize();
	return 0;

}

/*	RIFERIMENTI
	************************************************************************

	https://code.visualstudio.com/docs/editor/codebasics#_fold-selection

*/
