/* ************************************************************************** */
// LIBRERIE

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/* ************************************************************************** */

// DEFINIZIONE DI FUNZIONI E TIPI ACCESSORI

int scegliOperazione(int lim_sup) {
	int scelta_intero = 0, lim_inf = 1;
	
	char *buffer = NULL;
	size_t bufsize = 0;
	ssize_t chars_read;

	// buffer = malloc(bufsize * sizeof(char));
	// if(buffer == NULL) {
	// 	printf("Errore di allocazione della memoria!");
	// 	exit(1);
	// }

	chars_read = getline(&buffer, &bufsize, stdin); // inizializzazione del buffer con caratteri estratti dallo stream di input
	
	if (chars_read < 0) {
		printf("Errore nella lettura dell'input!");
		free(buffer);
		exit(1);
	} else {
		int digit_val = 0;
		for (int i = 0; i < chars_read; i++) {
			if (isdigit(buffer[i])) {
				digit_val = 10 * digit_val + (buffer[i] - '0'); // per ottenere il valore intero del carattere ASCII
				scelta_intero = scelta_intero + digit_val;
			} else {
				scelta_intero = 0;
				break;
			}
		}
	}

	printf("scelta_intero: %d\n", scelta_intero);
	// se scelta_intero == 0, allora il suo valore non è stato calcolato dal controllo di isdigit(buffer[i])
	if(!(scelta_intero >= lim_inf && scelta_intero <= lim_sup)) {
		printf("Puoi inserire solo un valore numerico intero compreso tra %d e %d!\n", lim_inf, lim_sup);
		printf("Applicazione terminata.\n");
		free(buffer);
		exit(1);
	}

	printf("\n");
	free(buffer);
	return scelta_intero;
}

/* ************************************************************************** */

int main(int argc, char const *argv[]) {

	/* ************************************************************************ */
	// DEFINIZIONE DELLE VARIABILI

	int scelta = 0, cont = 0;

	/* ************************************************************************ */

	// INTRODUZIONE

	printf("\n");
	printf("Buongiorno! Benvenuto nell'applicazione di testing per le\n");
	printf("esercitazioni di Parallel and Distributed Computing 2023-2024\n\n");

	/* ************************************************************************ */

	printf("Scegli un'operazione da effettuare: \n");
	printf("%d. \t Applicazione della strategia 1.\n", ++cont);
	printf("%d. \t Applicazione della strategia 2.\n", ++cont);
	printf("%d. \t Applicazione della strategia 3.\n", ++cont);
	printf("%d. \t Esecuzione dell'esempio d'uso (somma di 1).\n", ++cont);
	printf("%d. \t Chiudere l'applicazione.\n", ++cont);

  	scelta = scegliOperazione(cont);

	/* ************************************************************************ */

  	switch (scelta) {
		case 1: // Applicazione della strategia 1.
		{
			// ...
			break;
		}
		case 2: // Applicazione della strategia 2.
		{
			// ...
			break;
		}
		case 3: // Applicazione della strategia 3.
		{
			// ...
			break;
		}
		case 4: // Esecuzione dell'esempio d'uso (somma di 1).
		{
			// ...
			break;
		}
		case 5: // Chiudere l'applicazione
		{
			break;
		}
		default:
		{
			printf("Comando non riconosciuto!");
			break;
		}
	}

  	/* ************************************************************************ */

	printf("Applicazione terminata.\n");
	return 0;
}

/* ************************************************************************** */

/* RIFERIMENTI

https://stackoverflow.com/questions/35890237/how-to-create-a-type-definition-for-a-string-in-c
https://stackoverflow.com/questions/5029840/convert-char-to-int-in-c-and-c
https://opensource.com/article/22/5/safely-read-user-input-getline

*/