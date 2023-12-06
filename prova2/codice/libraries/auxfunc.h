/*

	auxfunc.h
	di Mario Gabriele Carofano
	e Francesco Noviello

*/

/* **************************************************************************** */
// DIRETTIVE E LIBRERIE

#ifndef AUXFUNCTIONS_H
#define AUXFUNCTIONS_H

/* **************************************************************************** */
// DEFINIZIONE DELLE FUNZIONI

int argToInt(char *arg);

double argToDouble(char *arg);

void writeTimeCSV(int test, int strategia, int n_proc, int q_num, double t_tot);

int getRowsFromCSV(const char* path);

int getColsFromCSV(const char* path);

void getMatrixFromCSV(const char* path, double** mat, int rows_mat, int cols_mat);

void getVectorFromCSV(const char* path, double* vet, int cols_mat);

void freeMatrix(double** mat, int rows);

/* **************************************************************************** */

#include "auxfunc.c"
#endif

/* **************************************************************************** */
/* RIFERIMENTI



*/
