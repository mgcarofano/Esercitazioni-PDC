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

void createPBS(int scelta, int q_num, int time_calc);

void printFile(FILE *f);

/* **************************************************************************** */

#include "menufunc.c"
#endif

/* **************************************************************************** */
/* RIFERIMENTI



*/
