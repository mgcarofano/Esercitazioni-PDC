/*

	csvfunc.h
	di Mario Gabriele Carofano
	e Francesco Noviello

*/

/*	*************************************************************************** */
// DIRETTIVE E LIBRERIE

#ifndef CSVFUNCTIONS_H
#define CSVFUNCTIONS_H

#include <stdio.h>

/*	*************************************************************************** */
// DEFINIZIONE DELLE FUNZIONI

void getDimensionsFromCSV(FILE* csv_file, int* size, int* rows_csv, int* cols_csv);

void getMatrixFromCSV(const char* path, double* mat, int rows_mat, int cols_mat, MPI_Comm comm);

void writeTimeCSV(
	const char* out_path,
	int A_rows, int A_cols,
	int B_rows, int B_cols,
	int n_proc, int input, int test,
	double t_tot,
	MPI_Comm comm
);

/*	*************************************************************************** */

#include "csvfunc.c"
#endif

/*	***************************************************************************
	RIFERIMENTI

*/