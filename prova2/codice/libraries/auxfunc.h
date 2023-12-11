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

void writeTimeCSV(const char* path, int rows, int cols, int threads, int test, double t_tot);

void freeMatrix(double** mat, int rows);

int getRowsFromCSV(const char* path);

int getColsFromCSV(const char* path);

// void getCSVDimensions(const char* path, int* rows_csv, int* cols_csv);

void getMatrixFromCSV(const char* path, double** mat, int rows_mat, int cols_mat);

void getVectorFromCSV(const char* path, double* vet, int cols_mat);

/* **************************************************************************** */

#include "auxfunc.c"
#endif

/* **************************************************************************** */
/* RIFERIMENTI



*/
