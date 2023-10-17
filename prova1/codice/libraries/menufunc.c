/* **************************************************************************** */
// LIBRERIE

#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <fcntl.h>

/* **************************************************************************** */
// CODICI DI ERRORE

#define INPUT_LINE_ERROR 5
#define NOT_REAL_NUMBER_ERROR 6
#define NOT_NATURAL_NUMBER_ERROR 7
#define NOT_IN_RANGE_ERROR 8
#define NOT_ENOUGH_OPERANDS 9
#define FILE_OPENING_ERROR 10

/* **************************************************************************** */
// CODICE DELLE FUNZIONI DEFINITE IN 'menufunc.h'

void printTitle() {
	printf("################################################################\n");
	printf("#      ________      __                     __           ____  #\n");
	printf("#     / ____/ /___ _/ /_  ____  _________ _/ /_____     /  _/  #\n");
	printf("#    / __/ / / __ `/ __ \\/ __ \\/ ___/ __ `/ __/ __ \\    / /    #\n");
	printf("#   / /___/ / /_/ / /_/ / /_/ / /  / /_/ / /_/ /_/ /  _/ /     #\n");
	printf("#  /_____/_/\\__,_/_.___/\\____/_/   \\__,_/\\__/\\____/  /___/     #\n");
	printf("#                                                              #\n");
	printf("# ---------- Calcolo della somma di N numeri reali ----------- #\n");
	printf("#                                                              #\n");
	printf("################################################################\n");
	printf("\n");
}

double getNumberFromInput() {
	double out = 0;
	char *buffer = NULL;
	size_t bufsize = 0;
	ssize_t chars_read;

	chars_read = getline(&buffer, &bufsize, stdin); // inizializzazione del buffer con caratteri estratti dallo stream di input
	printf("\n");

	if (chars_read < 0) {

		printf("Errore nella lettura dell'input!");
		printf("Applicazione terminata.\n");
		free(buffer);
		exit(INPUT_LINE_ERROR);

	} else {

		double digit_val = 0.0;
		int i = 0;
		int exp_whole = 0;
		int exp_fract = 0;
		int pos_point = -1;

		// si inizia da chars_read-2 perché:
		// -	la numerazione del buffer è da 0 a N-1
		// -	l'ultimo carattere è \0

		for (i = chars_read-2; i >= 0; i--) {
			if (buffer[i] != '.' && buffer[i] != ',') {
				exp_fract--;
			} else {
				pos_point = chars_read - (exp_fract * (-1)) -2;
				break;
			}
		}

		// printf("--- chars_read: %zd ---\n", chars_read);
		// printf("--- pos_point: %d ---\n\n", pos_point);

		i = chars_read-2;
		while(i >= 0) {
			// printf("--- i: %d ---\n", i);
			// printf("--- buffer[i]: %c ---\n", buffer[i]);
			// printf("--- exp_whole: %d ---\n", exp_whole);
			// printf("--- exp_fract: %d ---\n", exp_fract);

			if (i > pos_point && pos_point != -1) {

				if (isdigit(buffer[i])) {
					// printf("--- pow(10, exp_fract): %f ---\n", pow(10, exp_fract));
					// printf("--- digit_val: %f ---\n\n", digit_val);
					digit_val = pow(10, exp_fract) * (digit_val + (buffer[i] - '0')); // per ottenere il valore intero del carattere ASCII
					out = out + digit_val;
					// printf("--- out: %f ---\n", out);
					// printf("--- digit_val: %f ---\n\n", digit_val);
				} else {
					out = 0.0;
					printf("Puoi inserire solo valori numerici reali!\n");
					printf("Applicazione terminata.\n");
					free(buffer);
					exit(NOT_REAL_NUMBER_ERROR);
				}

				exp_fract++;

			} else if (i < pos_point || pos_point == -1) {

				if (isdigit(buffer[i])) {
					// printf("--- pow(10, exp_fract): %f ---\n", pow(10, exp_fract));
					// printf("--- digit_val: %f ---\n\n", digit_val);
					digit_val = pow(10, exp_whole) * (digit_val + (buffer[i] - '0'));
					out = out + digit_val;
					// printf("--- out: %f ---\n", out);
					// printf("--- digit_val: %f ---\n\n", digit_val);
				} else {
					if (i == 0 && buffer[0] == '-') {
						out = out * (-1);
					} else {
						out = 0.0;
						printf("Puoi inserire solo valori numerici reali!\n");
						printf("Applicazione terminata.\n");
						free(buffer);
						exit(NOT_REAL_NUMBER_ERROR);
					}
				}

				exp_whole++;
			}

			digit_val = 0.0;
			i--;
		}
	}

	free(buffer);
	return out;
}

int getIntegerFromInput() {

	double out_double = 0.0;
	int out_integer = 0;
	
	out_double = getNumberFromInput();
	out_integer = (int)out_double;

	if (out_integer < 0) {
		printf("Puoi inserire solo valori numerici naturali!\n");
		printf("Applicazione terminata.\n");
		exit(NOT_NATURAL_NUMBER_ERROR);
	}

	return out_integer;
}

void checkScelta(int scelta, int lim_inf, int lim_sup) {
	if(!(scelta >= lim_inf && scelta <= lim_sup)) {
		printf("Puoi inserire solo un valore numerico intero compreso tra %d e %d!\n", lim_inf, lim_sup);
		printf("Applicazione terminata.\n");
		exit(NOT_IN_RANGE_ERROR);
	}

	return;
}

void printFile(FILE *f) {
	char char_to_read;

	do {
		char_to_read = fgetc(f);
		if (char_to_read != EOF) printf("%c", char_to_read);
	} while (char_to_read != EOF);
}

/* **************************************************************************** */
/* RIFERIMENTI



*/
