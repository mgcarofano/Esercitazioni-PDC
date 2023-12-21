/*

	csvfunc.h
	di Mario Gabriele Carofano
	e Francesco Noviello

*/

/*	*************************************************************************** */
// DIRETTIVE E LIBRERIE

#ifndef CSVFUNCTIONS_H
#define CSVFUNCTIONS_H

/*	*************************************************************************** */
// DEFINIZIONE DELLE FUNZIONI

void getCSVDimensions(const char* path, int* rows_csv, int* cols_csv);

void getMatrixFromCSV(const char* path, double* mat, int rows_mat, int cols_mat);

// void writeTimeCSV(const char* path, int rows, int cols, int threads, int test, double t_tot);

/*	*************************************************************************** */

void read_matrix(char* filecsv, double **matrix, int *rows, int *cols, int world_rank, int world_size);

#include "csvfunc.c"
#endif

/*	***************************************************************************
	RIFERIMENTI

*/
