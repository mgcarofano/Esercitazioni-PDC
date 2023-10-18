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

void createPBS(int n_proc, int scelta, int q_num, int test, int time_calc, const char* path);

/* **************************************************************************** */

#include "menufunc.c"
#endif

/* **************************************************************************** */
/* RIFERIMENTI



*/
