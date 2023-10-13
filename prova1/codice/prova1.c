/* ************************************************************************** */
// LIBRERIE

// #include "mpi.h"
#include <stdio.h>

/* ************************************************************************** */
// ENUMERAZIONI E COSTANTI

#define NOME_PROVA "prova1"

/* ************************************************************************** */
// DEFINIZIONE DI FUNZIONI E TIPI ACCESSORI



/* ************************************************************************** */

int main(int argc, char const *argv[]) {

	/* ************************************************************************ */
	// DEFINIZIONE DELLE VARIABILI

	int scelta = argv[1];
	int q_num = argv[2];

	/* ************************************************************************ */

	// INTRODUZIONE

	printf("Inizio esecuzione.\n\n");

	/* ************************************************************************ */

  	switch (scelta) {
		case 1: // Applicazione della strategia 1.
		{
			printf("Applicazione della strategia 1.\n");
			break;
		}
		case 2: // Applicazione della strategia 2.
		{
			printf("Applicazione della strategia 2.\n");
			break;
		}
		case 3: // Applicazione della strategia 3.
		{
			printf("Applicazione della strategia 3.\n");
			break;
		}
		case 4: // Esecuzione dell'esempio d'uso (somma di 1).
		{
			printf("Esecuzione dell'esempio d'uso (somma di 1).\n");
			break;
		}
		default:
		{
			printf("Comando non riconosciuto!\n");
			break;
		}
	}

  	/* ************************************************************************ */

	printf("%d", q_num);
	printf("\nEsecuzione terminata.\n");
	return 0;
}

/* ************************************************************************** */

/* RIFERIMENTI

https://stackoverflow.com/questions/35890237/how-to-create-a-type-definition-for-a-string-in-c
https://stackoverflow.com/questions/5029840/convert-char-to-int-in-c-and-c
https://opensource.com/article/22/5/safely-read-user-input-getline
https://stackoverflow.com/questions/19209141/how-do-i-execute-a-shell-built-in-command-with-a-c-function
https://www.w3schools.com/c/c_files_write.php
https://stackoverflow.com/questions/5256313/c-c-macro-string-concatenation

*/
