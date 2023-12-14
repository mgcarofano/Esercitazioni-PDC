/*

	menufunc.h
	di Mario Gabriele Carofano
	e Francesco Noviello

*/

/* **************************************************************************** */
// DIRETTIVE E LIBRERIE

#ifndef MENUFUNCTIONS_H
#define MENUFUNCTIONS_H

#include <stdio.h>

/* **************************************************************************** */
// DEFINIZIONE DELLE FUNZIONI

void printTitle();

double getNumberFromInput();

int getIntegerFromInput();

void checkScelta(int scelta, int lim_inf, int lim_sup);

void createPBS(int rows, int cols, int threads, int test, int time_calc, int pbs_count);

/* **************************************************************************** */

#include "menufunc.c"
#endif

/* **************************************************************************** */
/* RIFERIMENTI



*/
