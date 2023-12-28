/*

	auxfunc.h
	di Mario Gabriele Carofano
	e Francesco Noviello

*/

/*	**************************************************************************** */
//	DIRETTIVE E LIBRERIE

#ifndef AUXFUNCTIONS_H
#define AUXFUNCTIONS_H

#include "mpi.h"
#include <stdio.h>

/*	**************************************************************************** */
//	DEFINIZIONE DELLE FUNZIONI

int argToInt(char *arg);

double argToDouble(char *arg);

void getRandomMatrix(double *mat, int mat_rows, int mat_cols);

void getIdentityMatrix(double *mat, int mat_rows, int mat_cols);

void getTransposeMatrix(double *A_mat, int A_rows, int A_cols, double *B_mat, int B_rows, int B_cols);

double* scatterMatrixToGrid(
	double* mat, int mat_rows, int mat_cols,
	int* loc_rows, int* loc_cols,
	int id_grid, int* grid_dim, int* grid_coords, MPI_Comm comm_grid
);

void fprintfMatrix(FILE* out_file, double* mat, int rows, int cols, const char* format);

void bmr_broadcast(
	double* mat, double* tmp,
	int rows, int cols,
	int step,
	MPI_Comm comm_grid, MPI_Comm comm_row
);

void bmr_multiply(
	double* A_mat, double* B_mat, double* C_mat,
	int A_rows, int A_cols,
	int B_rows, int B_cols,
	int C_rows, int C_cols,
	MPI_Comm comm
);

void bmr_rolling(
	double* mat, double* tmp,
	int rows, int cols,
	MPI_Comm comm_col
);

void bmr(
	double* A_mat, double* B_mat, double* C_mat,
	int A_rows, int A_cols,
	int B_rows, int B_cols,
	int C_rows, int C_cols,
	int *grid_dim,
	MPI_Comm comm, MPI_Comm comm_row, MPI_Comm comm_col 
);

/*	**************************************************************************** */

#include "auxfunc.c"
#endif

/*	****************************************************************************
	RIFERIMENTI

*/
