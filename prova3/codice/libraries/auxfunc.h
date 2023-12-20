/*

	auxfunc.h
	di Mario Gabriele Carofano
	e Francesco Noviello

*/

/*	**************************************************************************** */
//	DIRETTIVE E LIBRERIE

#ifndef AUXFUNCTIONS_H
#define AUXFUNCTIONS_H

/*	**************************************************************************** */
//	DEFINIZIONE DELLE FUNZIONI

int argToInt(char *arg);

double argToDouble(char *arg);

void getRandomMatrix(double *mat, int mat_rows, int mat_cols);

void getIdentityMatrix(double *mat, int mat_rows, int mat_cols);

void getTransposeMatrix(double *A_mat, int A_rows, int A_cols, double *B_mat, int B_rows, int B_cols);

/*	**************************************************************************** */

#include "auxfunc.c"
#endif

/*	****************************************************************************
	RIFERIMENTI



*/
