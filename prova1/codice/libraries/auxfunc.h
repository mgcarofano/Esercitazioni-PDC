/* **************************************************************************** */
// DIRETTIVE E LIBRERIE

#ifndef AUXFUNCTIONS_H
#define AUXFUNCTIONS_H

#include <stdio.h>

/* **************************************************************************** */
// DEFINIZIONE DELLE FUNZIONI

void printTitle();

double getNumberFromInput();

int getIntegerFromInput();

void checkScelta(int scelta, int lim_inf, int lim_sup);

void printFile(FILE *f);

int argToInt(char *arg);

double argToDouble(char *arg);

/* **************************************************************************** */

#include "auxfunc.c"
#endif

/* **************************************************************************** */
/* RIFERIMENTI



*/
