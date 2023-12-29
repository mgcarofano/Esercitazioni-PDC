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
	int pbs_count = 0, check_test = 1;

	int i = 0, j = 0, k = 0;

	double *A_mat = NULL, *B_mat = NULL;
	int A_rows = 0, A_cols = 0;
	int B_rows = 0, B_cols = 0;

	double *loc_A_mat = NULL, *loc_B_mat = NULL, *loc_C_mat = NULL;
	int loc_A_rows = 0, loc_A_cols = 0;
	int loc_B_rows = 0, loc_B_cols = 0;
	int loc_C_rows = 0, loc_C_cols = 0;

	double time_start = 0.0, time_end = 0.0;
	double time_loc = 0.0, time_tot = 0.0;

	double grid_tmp = 0;
	int n_proc = 0, id_proc = 0;
	int grid_rows = 0, grid_cols = 0;
	int *grid_dim = NULL, *period = NULL, *grid_coords = NULL, *remain_dims = NULL;
	
	FILE *csv_A_mat = NULL, *csv_B_mat = NULL, *out_file = NULL;
	char out_path[PATH_MAX_LENGTH] = {};

	MPI_Comm comm, comm_row, comm_col;

	srand(time(NULL));

	//	INIZIALIZZAZIONE DELL'AMBIENTE MPI
	/*	*********************************************************************** */

	comm = MPI_COMM_WORLD;

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
		i processori attivati.
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

	if (id_proc == 0 && DEBUG) printf("Inizio esecuzione.\n");

	if (id_proc == 0) {

		/*
			Si affida al primo processore con id_proc == 0 il compito
			di leggere gli argomenti in input 'argv[]'.
		*/

		if (argc < ARGS_QUANTITY) {
			printf("Errore nella lettura degli argomenti di input!\n");
			printf("Sono richiesti almeno %d argomenti in input!\n", ARGS_QUANTITY);
			printf("Esecuzione terminata.\n");
			MPI_Abort(comm, NOT_ENOUGH_ARGS_ERROR);
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
			MPI_Abort(comm, MATRIX_DIMENSION_ERROR);
		}

		grid_tmp = sqrt(n_proc);

		if (n_proc != 0 && grid_tmp != floor(grid_tmp)) {
			printf("Il numero di processi (%d) non e' un quadrato perfetto.\n", n_proc);
			printf("Impossibile costruire una griglia di processi bidimensionale quadrata.\n");
			printf("Esecuzione terminata.\n");
			MPI_Abort(comm, PROCESSOR_QUANTITY_ERROR);
		}

		grid_rows = grid_cols = floor(grid_tmp);

		if ((A_cols % grid_cols != 0) || (B_cols % grid_cols != 0)) {
			printf("Le matrici non sono compatibili con la griglia di processori!\n");
			printf("Esecuzione terminata.\n");
			MPI_Abort(comm, MATRIX_DIMENSION_ERROR);
		}

		input = argToInt(argv[5]);
		test = argToInt(argv[6]);
		time_calc = argToInt(argv[7]);

		pbs_count = argToInt(argv[8]);

	}

	if (id_proc == 0 && DEBUG) printf("Lettura degli argomenti completata.\n");

	MPI_Bcast(&A_rows, 1, MPI_INT, 0, comm);
	MPI_Bcast(&A_cols, 1, MPI_INT, 0, comm);
	MPI_Bcast(&B_rows, 1, MPI_INT, 0, comm);
	MPI_Bcast(&B_cols, 1, MPI_INT, 0, comm);

	MPI_Bcast(&grid_rows, 1, MPI_INT, 0, comm);
	MPI_Bcast(&grid_cols, 1, MPI_INT, 0, comm);

	MPI_Bcast(&input, 1, MPI_INT, 0, comm);
	MPI_Bcast(&test, 1, MPI_INT, 0, comm);
	MPI_Bcast(&time_calc, 1, MPI_INT, 0, comm);

	MPI_Bcast(&pbs_count, 1, MPI_INT, 0, comm);

	if (id_proc == 0 && DEBUG) printf("Distribuzione degli argomenti completata.\n");

	//	CREAZIONE DELLA GRIGLIA BIDIMENSIONALE
	/*	******************************************************************** */

	if (n_proc > 1) {

		grid_coords		= (int*) calloc(2, sizeof(int));
		grid_dim		= (int*) calloc(2, sizeof(int));
		period			= (int*) calloc(2, sizeof(int));
		remain_dims		= (int*) calloc(2, sizeof(int));

		grid_dim[0] = grid_rows;
		grid_dim[1] = grid_cols;

		period[0] = 1;
		period[1] = 1;

		MPI_Cart_create(MPI_COMM_WORLD, 2, grid_dim, period, 0, &comm);
		MPI_Cart_coords(comm, id_proc, 2, grid_coords);

		remain_dims[0] = 0;
		remain_dims[1] = 1;
		MPI_Cart_sub(comm, remain_dims, &comm_row);
	
		remain_dims[0] = 1;
		remain_dims[1] = 0;
		MPI_Cart_sub(comm, remain_dims, &comm_col);

	}

	if (id_proc == 0 && DEBUG) printf("Creazione della griglia completata.\n");

	//	CREAZIONE DEI FILE DI OUTPUT
	/*	******************************************************************** */

	if (n_proc == 1) {
		sprintf(out_path, NOME_PROVA "/output/" NOME_PROVA "_%03d/proc%d.out",
			pbs_count, id_proc
		);
	} else {
		sprintf(out_path, NOME_PROVA "/output/" NOME_PROVA "_%03d/proc%d_%02d_%02d.out",
			pbs_count, id_proc, grid_coords[0], grid_coords[1]
		);
	}
	
	if ((out_file = fopen(out_path, "w")) == NULL) {
		printf("Nessun file o directory con questo nome: %s\n", out_path);
		printf("Esecuzione terminata.\n");
		MPI_Abort(comm, FILE_OPENING_ERROR);
	}

	if (id_proc == 0 && DEBUG) printf("File di output creato.\n");

	//	LETTURA E/O GENERAZIONE DEGLI ELEMENTI DELLA MATRICE
	/*	******************************************************************** */

	if (id_proc == 0) {

		A_mat = (double*) calloc(A_rows * A_cols, sizeof(double));
		B_mat = (double*) calloc(B_rows * B_cols, sizeof(double));

		if (!A_mat && !B_mat) {
			printf("Le matrici globali non sono state allocate correttamente!\n");
			printf("Esecuzione terminata.\n");
			MPI_Abort(comm, ALLOCATION_ERROR);
		}

		switch (test) {
			case DEFAULT_TEST:
			{
				switch (input) {
					case VALUES_FROM_INPUT:
					{
						if (((A_rows*A_cols)+(B_rows*B_cols)) <= OP_MAX_QUANTITY) {
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
						if ((csv_A_mat = fopen(argv[ARGS_QUANTITY+1], "r")) == NULL) {
							printf("Nessun file o directory con questo nome: %s\n", argv[ARGS_QUANTITY+1]);
							printf("Applicazione terminata.\n");
							MPI_Abort(comm, FILE_OPENING_ERROR);
						}

						if ((csv_B_mat = fopen(argv[ARGS_QUANTITY+2], "r")) == NULL) {
							printf("Nessun file o directory con questo nome: %s\n", argv[ARGS_QUANTITY+2]);
							printf("Applicazione terminata.\n");
							MPI_Abort(comm, FILE_OPENING_ERROR);
						}

						getMatrixFromCSV(csv_A_mat, A_mat, A_rows, A_cols, comm);
						getMatrixFromCSV(csv_B_mat, B_mat, B_rows, B_cols, comm);

						if (fclose(csv_A_mat) != 0) {
							printf("Nessun file o directory con questo nome: %s\n", argv[ARGS_QUANTITY+1]);
							printf("Applicazione terminata.\n");
							MPI_Abort(comm, FILE_CLOSING_ERROR);
						}

						if (fclose(csv_B_mat) != 0) {
							printf("Nessun file o directory con questo nome: %s\n", argv[ARGS_QUANTITY+2]);
							printf("Applicazione terminata.\n");
							MPI_Abort(comm, FILE_CLOSING_ERROR);
						}

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
			case MULTIPLICATION_TRACE_TEST:
			{
				getRandomMatrix(A_mat, A_rows, A_cols);
				getRandomMatrix(B_mat, B_rows, B_cols);
				break;
			}
			default:
				break;
		}

		if (id_proc == 0 && DEBUG) printf("Generazione delle matrici globali completata.\n");

		if (n_proc > 1) fprintf(out_file, "\n--- MATRICI GLOBALI ---\n");
		fprintf(out_file, "\nMatrice A di dimensione %d x %d:\n", A_rows, A_cols);
		fprintfMatrix(out_file, A_mat, A_rows, A_cols, "%f");

		fprintf(out_file, "\nMatrice B di dimensione %d x %d:\n", B_rows, B_cols);
		if (test == MULTIPLICATION_IDENTITY_TEST) {
			fprintfMatrix(out_file, B_mat, B_rows, B_cols, "%1.0f");
		} else {
			fprintfMatrix(out_file, B_mat, B_rows, B_cols, "%f");
		}
		if (n_proc > 1) fprintf(out_file, "\n-----\n");

	}

	//	DISTRIBUZIONE DELLE MATRICI
	/*	******************************************************************** */

	if (n_proc == 1) {

		loc_A_mat = A_mat;
		loc_A_rows = A_rows;
		loc_A_cols = A_cols;
		
		loc_B_mat = B_mat;
		loc_B_rows = B_rows;
		loc_B_cols = B_cols;

	} else {

		loc_A_mat = scatterMatrixToGrid(
			A_mat, A_rows, A_cols,
			&loc_A_rows, &loc_A_cols,
			id_proc, grid_dim, grid_coords, comm
		);

		loc_B_mat = scatterMatrixToGrid(
			B_mat, B_rows, B_cols,
			&loc_B_rows, &loc_B_cols,
			id_proc, grid_dim, grid_coords, comm
		);

		if (id_proc == 0) fprintf(out_file, "\n--- MATRICI LOCALI ---\n");
		fprintf(out_file, "\nMatrice loc_A di dimensione %d x %d:\n", loc_A_rows, loc_A_cols);
		fprintfMatrix(out_file, loc_A_mat, loc_A_rows, loc_A_cols, "%f");

		fprintf(out_file, "\nMatrice loc_B di dimensione %d x %d:\n", loc_B_rows, loc_B_cols);
		if (test == MULTIPLICATION_IDENTITY_TEST) {
			fprintfMatrix(out_file, loc_B_mat, loc_B_rows, loc_B_cols, "%1.0f");
		} else {
			fprintfMatrix(out_file, loc_B_mat, loc_B_rows, loc_B_cols, "%f");
		}
		if (id_proc == 0) fprintf(out_file, "\n-----\n");

	}

	if (id_proc == 0 && DEBUG) printf("Distribuzione delle matrici completata.\n");

	/*
	
		Non appena termina con successo la distribuzione delle matrici
		"A_mat" e "B_mat" dal processore radice a tutti gli altri processori
		della griglia, si puo' liberare la memoria allocata.

	*/

	if (n_proc > 1 && id_proc == 0) {
		free(A_mat);
		free(B_mat);
	}

	if (id_proc == 0 && DEBUG) printf("Pulizia della memoria completata.\n");
	
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

		MPI_Barrier(comm);

		/*
			--- double MPI_Wtime() ---
			Si utilizza per ottenere un valore di tempo in secondi
			rispetto ad un tempo arbitrario nel passato.
		*/

		time_start = MPI_Wtime();
	}

	//	APPLICAZIONE DEL BROADCAST MULTIPLY ROLLING PER IL CALCOLO DEL PRODOTTO MATRICE-MATRICE
	/*	******************************************************************** */

	loc_C_rows = loc_A_rows;
	loc_C_cols = loc_B_cols;
	loc_C_mat = (double*) calloc(loc_C_rows * loc_C_cols, sizeof(double));

	if (!loc_C_mat) {
		printf("La matrice del risultato non e' stata allocata correttamente!\n");
		printf("Esecuzione terminata.\n");
		MPI_Abort(comm, ALLOCATION_ERROR);
	}

	bmr(
		loc_A_mat, loc_B_mat, loc_C_mat,
		loc_A_rows, loc_A_cols,
		loc_B_rows, loc_B_cols,
		loc_C_rows, loc_C_cols,
		comm, comm_row, comm_col 
	);

	if (id_proc == 0 && DEBUG) printf("Applicazione dell'algoritmo BMR completato.\n");

	//	SALVATAGGIO DEL CALCOLO DEI TEMPI DI ESECUZIONE
	/*	******************************************************************** */

	if (time_calc == OK_TIME_CALC) {

		MPI_Barrier(comm);
		time_end = MPI_Wtime();

		// Si calcola la distanza di tempo tra l'istante iniziale e quello finale.
		time_loc = time_end - time_start;

		/*
			--- int MPI_Reduce(void *sendbuf, void *recvbuf, int count,
    				MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm) ---
			Si utilizza questa funzione per eseguire un'operazione collettiva
			su tutti i processori del contesto. In questo caso, si desidera 
			calcolare il massimo tempo tra tutti i tempi calcolati localmente.
		*/

		MPI_Reduce(&time_loc, &time_tot, 1, MPI_DOUBLE, MPI_MAX, 0, comm);

		if (id_proc == 0) {

			// Si veda il sezione "Futuri sviluppi" della documentazione.
			// switch (test) {
			// 	case MULTIPLICATION_IDENTITY_TEST:
			// 	{
			// 		// ...
			// 		break;
			// 	}
			// 	case MULTIPLICATION_TRANSPOSE_TEST:
			// 	{
			// 		// ...
			// 		break;
			// 	}
			// 	case MULTIPLICATION_TRACE_TEST:
			// 	{
			// 		// ...
			// 		break;
			// 	}
			// 	default:
			// 		break;
			// }

			if (check_test) {

				if (id_proc == 0 && DEBUG)
					printf("\nCalcolo del prodotto matrice-matrice terminato in %e sec\n", time_tot);

				writeTimeCSV(
					CSV_TIME_PATH"/"NOME_PROVA"_time.csv",
					A_rows, A_cols, B_rows, B_cols,
					n_proc, input, test,
					time_tot,
					comm
				);
			}
		}
	}

	//	STAMPA DELL'OUTPUT
  	/*	******************************************************************** */

	// Si veda il sezione "Futuri sviluppi" della documentazione.
	// if (id_proc == 0) {

	// 	if (n_proc == 1) {
	// 		C_mat = loc_C_mat;
	// 		C_rows = A_rows;
	// 		C_cols = B_cols;
	// 	} else {
	// 		// ...
	// 	}

	// 	fprintf(out_file, "\nRisultato globale:");
	// 	fprintf(out_file, "\nMatrice C di dimensione %d x %d:\n", C_rows, C_cols);
	// 	fprintfMatrix(out_file, C_mat, C_rows, C_cols, "%f");

	// 	free(C_mat);
	// }

	fprintf(out_file, "\nRisultato locale:");
	fprintf(out_file, "\nMatrice loc_C di dimensione %d x %d:\n", loc_C_rows, loc_C_cols);
	fprintfMatrix(out_file, loc_C_mat, loc_C_rows, loc_C_cols, "%f");

	if (id_proc == 0 && DEBUG) printf("Stampa dell'output completata.\n");

	//	TERMINAZIONE DELL'ESECUZIONE
  	/*	******************************************************************** */

	/*
		Attendiamo che tutti i processori abbiano portato a termine
		correttamente il loro carico di lavoro.
	*/

	MPI_Barrier(comm);

	// Al termine dell'esecuzione, si libera lo spazio allocato in memoria.
	free(loc_A_mat);
	free(loc_B_mat);
	free(loc_C_mat);

	if (n_proc > 1) {
		free(grid_coords);
		free(grid_dim);
		free(period);
		free(remain_dims);
	}

	if (fclose(out_file) != 0) {
		printf("Nessun file o directory con questo nome: %s\n", out_path);
		printf("Esecuzione terminata.\n");
		MPI_Abort(comm, FILE_CLOSING_ERROR);
	}

	if (id_proc == 0 && DEBUG) printf("\nEsecuzione terminata.\n");

	MPI_Finalize();
	return 0;

}

/*	RIFERIMENTI
	************************************************************************

	https://code.visualstudio.com/docs/editor/codebasics#_fold-selection
	https://pubs.opengroup.org/onlinepubs/007904975/functions/getcwd.html
	https://stackoverflow.com/questions/66397329/why-is-this-code-giving-a-stray-in-program-error
	
*/
