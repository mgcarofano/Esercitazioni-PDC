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

int getFieldsFromCSV(const char* path);

double** getMatrix(const char* path);

/* **************************************************************************** */

#include "auxfunc.c"
#endif

/* **************************************************************************** */
/* RIFERIMENTI



*/
