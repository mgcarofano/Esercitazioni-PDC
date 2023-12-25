/*

	prova3.c
	di Mario Gabriele Carofano
	e Francesco Noviello
    VERSIONE DI PROVA

*/

/* **************************************************************************** */
// LIBRERIE

#include "./libraries/auxfunc.h"
#include <mpi.h>

/* **************************************************************************** */
void random_matrix(int dim, double **mat);
void multiply(double **matA, double **matB, double **matResult, int dim);
double** initialize_matrix(int dim);
void send_matrix(double* matrix, int dim_matrix, int dim_submat, MPI_Comm grid, int mpi_size);
void receive_matrix(double* sub_matrix, int dim_submat, int mpi_rank);

//Riscrivo il codice non considerando il menù e la matrice come vera matrice...
int main(int argc, char **argv) {
	int i, j;
	int mpi_rank, mpi_size;
	double time_start, time_end, time_diff, time_seq;

	int dim_mat;
	double **matA, **matB, **matResult;

	int dim_submat;
	double **sub_matA, **sub_matB, **sub_matResult;

	int periods[2]={0};
	int coords[2];
	int n_proc;

	int dim_grid;
	MPI_Comm grid, sub_row_grid, sub_cols_grid;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

	if (mpi_rank == 0){ // Processore 0
		//Verifica degli argomenti passati in input
		//MPI_Abort(MPI_COMM_WORLD, 401); // 401 Errore no argomenti
		//MPI_Abort(MPI_COMM_WORLD, 402); // 402 Errore argomenti sbagliati
	}

	if (mpi_rank == 0){ // Processore 0
		dim_mat = argToInt(argv[1]); // dimensione delle matrici quadrate
		n_proc = argToInt(argv[2]); // numero processori
		if (n_proc == mpi_size){
			dim_grid = sqrt(n_proc); // dimensione griglia
		} else {
			//segnalare che il numero di processori non è uguale a mpi_size
			MPI_Abort(MPI_COMM_WORLD, 403); // 403 Errore n_proc != mpi_size
		}
		dim_submat = dim_mat / dim_grid; // dimensione sottomatrici
	}

	MPI_Bcast(&dim_mat, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&n_proc, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&dim_grid, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&dim_submat, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if (mpi_rank == 0){ // Processore 0
		srand(time(NULL));
		matA = initialize_matrix(dim_mat);
		matB = initialize_matrix(dim_mat);
		random_matrix(dim_mat, matA);
		random_matrix(dim_mat, matB);
		if (!matA || !matB){
			//segnalare che le matrici non sono state allocate
			MPI_Abort(MPI_COMM_WORLD, 404); // 404 Errore no memoria allocata
		}

	}

	if (mpi_size == 1){ // Singolo processore - Algoritmo sequenziale
		time_start = MPI_Wtime();

		matResult = initialize_matrix(dim_mat);
		if (!matResult){
			//segnalare che la matrice non è stata allocata
			MPI_Abort(MPI_COMM_WORLD, 404); // 404 Errore no memoria allocata
		}
		multiply(matA, matB, matResult, dim_mat);

		time_end = MPI_WTime();
		time_seq = time_end - time_start;

	} else { // Multiprocessore - Algoritmo parallelo
		//Creazione griglia
		create_grid(&grid, &sub_row_grid, &sub_cols_grid, mpi_rank, mpi_size, dim_grid, periods, 0, coords);

		//Creazione sottomatrici
		sub_matA = initialize_matrix(dim_submat);
		sub_matB = initialize_matrix(dim_submat);
		sub_matResult = initialize_matrix(dim_submat);

		if(mpi_rank == 0){ // Processore 0
			if(!sub_matA || !sub_matB || !sub_matResult){
				//segnalare che le matrici non sono state allocate
				MPI_Abort(MPI_COMM_WORLD, 404); // 404 Errore no memoria allocata
			}
		}

		if(mpi_rank == 0){ // Processore 0 distribuisce le matrici
			send_matrix(matA, dim_mat, dim_submat, grid, mpi_size);
			send_matrix(matB, dim_mat, dim_submat, grid, mpi_size);
			for (i=0; i<dim_submat; i++){
				for (j=0; j<dim_submat; j++){
					sub_matA[i][j] = matA[i][j];
					sub_matB[i][j] = matB[i][j]; 
				}
			}
		} else { // Altri processori ricevono le matrici
			receive_matrix(sub_matA, dim_submat, mpi_rank);
			receive_matrix(sub_matB, dim_submat, mpi_rank);
		}

		MPI_Barrier(MPI_COMM_WORLD); // Sincronizzazzione processori
		time_start = MPI_WTime();

		// Strategia BMR
		broadcast_multiply_rolls(
			sub_matA, sub_matB, sub_matResult, dim_submat,
			grid, sub_row_grid, sub_cols_grid, dim_grid, mpi_rank);
		// Ricomposizione matrice finale
		merge(matResult, dim_mat, sub_matResult, dim_submat, grid, mpi_size, mpi_rank);

		// Calcolo tempo
		time_end = MPI_WTime();
		time_diff = time_end - time_start;
		MPI_Reduce(&time_diff, &time_seq, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

	}

	// Stampa matrici input e calcolata
	// Stampa tempo
	
	MPI_Finalize();

	return 0;
}

double** initialize_matrix(int dim){
	double** matrix = (double**) calloc(dim, sizeof(double));
	for (int i = 0; i < dim; i++) {
        matrix[i] = (double *) calloc(dim,sizeof(double));
    }
	return matrix;
}

void random_matrix(int dim, double **mat){
	int i,j;
    for (i = 0; i < dim; i++) {
        for (j = 0; j < dim; j++) {
            double double_rand = (double)rand();
            int int_rand = (int)rand();

            // Si genera un numero casuale reale compreso tra 0 e 100
            mat[i][j] = (double_rand / RAND_MAX) * OP_MAX_VALUE;

            // Si ha il 33% di possibilita che mat[i][j] < 0
            if (int_rand % 3 == 0) {
                mat[i][j] = mat[i][j] * (-1);
            }
        }
    }
}

void multiply(double **matA, double **matB, double **matResult, int dim){
	int i,j,k;
	for(i = 0; i < dim; i++)
			for(j = 0; j < dim; j++)
				for(k = 0; k < dim; k++)
					matResult[i][j] += (matA[i][k] * matB[k][j]);
}

void create_grid(MPI_Comm* grid, MPI_Comm* sub_row_grid, MPI_Comm* sub_cols_grid, int mpi_rank, int mpi_size, int dim_grid, int* periods, int reorder, int* coords) {
	
	int dimensions[2] = {dim_grid, dim_grid};
	MPI_Cart_create(MPI_COMM_WORLD, 2, dimensions, periods, reorder, grid);
	MPI_Cart_coords(*grid, mpi_rank, 2, coords);
	int remains[2] = {0, 1};
	MPI_Cart_sub(*grid, remains, sub_row_grid);
	remains[0] = 1;
	remains[1] = 0;
	MPI_Cart_sub(*grid, remains, sub_cols_grid);

}

void send_matrix(double* matrix, int dim_matrix, int dim_submat, MPI_Comm grid, int mpi_size) {
	
	int coords[2];
	int start_row;
	int start_column;
	
	for(int processor = 1; processor < mpi_size; processor++) {
		MPI_Cart_coords(grid, processor, 2, coords);
		start_row = coords[0] * dim_submat;
		start_column = coords[1] * dim_submat;
		for(int row_offset = 0; row_offset < dim_submat; row_offset++)
			MPI_Send(&matrix[(start_row+row_offset)][start_column],dim_submat, MPI_DOUBLE, processor, processor, MPI_COMM_WORLD);
	}
}

void receive_matrix(double* sub_matrix, int dim_submat, int mpi_rank) {

	MPI_Status status;
	for(int row = 0; row < dim_submat; row++)
		MPI_Recv(&sub_matrix[row*dim_submat], dim_submat, MPI_DOUBLE,  0, mpi_rank, MPI_COMM_WORLD, &status);
}



/* **************************************************************************** */
/* RIFERIMENTI

https://stackoverflow.com/questions/9748393/how-can-i-get-argv-as-int

https://www.javatpoint.com/random-function-in-c
https://www.securitronlinux.com/bejiitaswrath/a-nice-example-of-c-programming-getting-a-random-number-in-milliseconds/?utm_content=cmp-true

https://www.educative.io/answers/how-to-check-if-a-number-is-a-power-of-2-in-cpp

http://www.vdepetris.it/t10/Text10.htm

*/

/*
void bmr( double* loc_A_mat, double* loc_B_mat, double* loc_C_mat, int loc_mat_dim,
	MPI_Comm grid, MPI_Comm grid_rows, MPI_Comm grid_cols, int grid_dim[2], int coords[2]) {	

	MPI_Request request;
	MPI_Status status;

	int loc_A_mat_broadcaster_coords[2];
	int loc_A_mat_broadcaster_rank;
	
	//Matrice di appoggio per il broadcasting in cui copio loc_A_mat
	
	double* tmp_matrix = initialize_matrix(loc_mat_dim);
			
	// Calcolo rank sotto griglia colonna del receiver della sotto matrice B
	
	int loc_B_mat_receiver_rank;
	int loc_B_mat_receiver_coords[2] = {
		(coords[0] + grid_dim[0] - 1) % grid_dim[0],
		coords[1]
	};
	MPI_Cart_rank(grid_cols, loc_B_mat_receiver_coords, &loc_B_mat_receiver_rank);
		
	// Calcolo rank sotto griglia colonna del sender della sotto matrice B 
	
	int loc_B_mat_sender_rank;
	int loc_B_mat_sender_coords[2] = {
		(coords[0] + 1) % grid_dim[0],
		loc_B_mat_sender_coords[1] = coords[1]
	};
    MPI_Cart_rank(grid_cols, loc_B_mat_sender_coords, &loc_B_mat_sender_rank);
    
    // Calcolo rank nella sotto griglia colonna del processore chiamante
    
	int rank_cgrid;
	MPI_Cart_rank(grid_cols, coords, &rank_cgrid);
	
	// Inizio BMR 
	
	for(int step = 0; step < grid_dim[0]; step++) {
	
		// Coordinate del processore che deve inviare la sotto matrice A
		
		loc_A_mat_broadcaster_coords[0] = coords[0];
	        loc_A_mat_broadcaster_coords[1] = (coords[0] + step) % grid_dim[0];
		
		
		if(!step) {		// Primo passo
			
			// Broadcasting
			
			if(coords[0] == coords[1]){
                		loc_A_mat_broadcaster_coords[1] = coords[1];
                		memcpy(tmp_matrix, loc_A_mat, loc_mat_dim*loc_mat_dim*sizeof(double));
            		}
			
			MPI_Cart_rank(grid_rows, loc_A_mat_broadcaster_coords, &loc_A_mat_broadcaster_rank);
			
			MPI_Bcast(tmp_matrix, loc_mat_dim*loc_mat_dim, MPI_DOUBLE, loc_A_mat_broadcaster_rank, grid_rows);
			
			// Moltiplica
			
		} else {	// Passi successivi
			
			// Broadcasting sulle diagonale superiori alla principale (k + step)
			
            if(coords[1] == loc_A_mat_broadcaster_coords[1]){
		    	memcpy(tmp_matrix, loc_A_mat, loc_mat_dim*loc_mat_dim*sizeof(double));
			}
            loc_A_mat_broadcaster_rank = (loc_A_mat_broadcaster_rank+1)%grid_dim[0];

            MPI_Bcast(tmp_matrix, loc_mat_dim*loc_mat_dim, MPI_DOUBLE, loc_A_mat_broadcaster_rank, grid_rows);

			// Rolling
            
			MPI_Isend(loc_B_mat, loc_mat_dim*loc_mat_dim, MPI_DOUBLE, loc_B_mat_receiver_rank, D_TAG + loc_B_mat_receiver_rank, grid_cols, &request);
			
			MPI_Recv(loc_B_mat, loc_mat_dim*loc_mat_dim, MPI_DOUBLE, loc_B_mat_sender_rank, D_TAG + rank_cgrid, grid_cols, &status);
		
			// Moltiplica
			
		}
		
	}
	
}







*/