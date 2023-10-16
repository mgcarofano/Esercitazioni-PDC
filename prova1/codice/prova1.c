/* **************************************************************************** */
// LIBRERIE

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <math.h>

/* **************************************************************************** */
// ENUMERAZIONI E COSTANTI



/* **************************************************************************** */
// DEFINIZIONE DI FUNZIONI E TIPI ACCESSORI

int argToInt(const char *arg) {
    
	char *p;
	errno = 0; // Si inizializza il valore di 'errno' nella libreria <errno.h>
    long out_long = strtol(arg, &p, 10); // Si utilizza questa funzione 'strtol' perche' consente un maggior numero di controlli

	if (strlen(arg) == 0) {
		printf("Errore nella lettura degli argomenti di input!\n\n");
		printf("Esecuzione terminata.\n");
		exit(1);
    }

	if (*p != '\0' || errno != 0) {
		printf("Errore nella lettura degli argomenti di input!\n\n");
		printf("Esecuzione terminata.\n");
		exit(1);
    }
    
    if (out_long < INT_MIN || out_long > INT_MAX) { // Per una corretta conversione da long a int, si controlla che il valore long sia rappresentabile come intero
        printf("Errore nella lettura degli argomenti di input!\n\n");
		printf("Esecuzione terminata.\n");
		exit(1);
    }
	
    return (int)out_long;
}

double argToDouble(const char *arg) {
    
	char *p;
	errno = 0;
    double out_double = strtod(arg, &p);

	if (strlen(arg) == 0) {
		printf("Errore nella lettura degli argomenti di input!\n\n");
		printf("Esecuzione terminata.\n");
		exit(1);
    }

	if (*p != '\0' || errno != 0) {
		printf("Errore nella lettura degli argomenti di input!\n\n");
		printf("Esecuzione terminata.\n");
		exit(1);
    }
	
    return out_double;
}

/* **************************************************************************** */

int main(int argc, char const *argv[]) {

	/* ************************************************************************ */
	// DEFINIZIONE DELLE VARIABILI

	int menum = 0, nproc = 0;
	int n = 0, nloc = 0, tag = 0, rest=0;
	int tmp = 0, start = 0;
	int *x, *xloc;

	// PORTO FUORI LE VARIABILI ALTRIMENTI CI SONO PROBLEMI CON LO SCOPE
	int scelta, q_num, time_calc;

	double sum = 0.0, sum_parz = 0.0;

	double t0 = 0.0, t1 = 0.0, time = 0.0;
	double timetot = 0.0;

	MPI_STATUS status;

	/* ************************************************************************ */
	// INTRODUZIONE

	printf("Inizio esecuzione.\n\n");

	/* ************************************************************************ */
	// LETTURA E DISTRIBUZIONE DEI DATI

	MPI_Init(&argv, &argc);
	MPI_Comm_rank(MPI_COMM_WORLD, &menum);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);

	if (menum == 0) {
		/* ******************************************************************** */
		// LETTURA DEGLI ARGOMENTI

		if (argc < 4) {
			printf("Errore nella lettura degli argomenti di input!\n\n");
			printf("Esecuzione terminata.\n");
			exit(1);
		}

		scelta = argToInt(argv[1]);
		q_num = argToInt(argv[2]);
		time_calc = argToInt(argv[3]);

		// printf("scelta: %d, q_num: %d, time_calc: %d\n\n", scelta, q_num, time_calc);

		double op[q_num];

		for (int i=0; i < q_num; i++) {
			op[i] = argToDouble(argv[i+4]);
			// printf("--- op[i]: %f ---\n", op[i]);
		}
	}

	MPI_Bcast(&q_num, 1, MPI_INT, 0, MPI_COMM_WORLD);

	nloc = n / nproc;
	rest = n % nproc;
	if (menum < rest) {
		nloc++;
	}

	// TODO: allocazione di xloc

	if (menum == 0) {
		xloc = x;
		tmp = nloc;
		for (int i=1; i < nproc; i++) {
			start = start + tmp;
			tag = i +22;
			if (i == rest) {
				tmp--;
			}

			MPI_Send(&x[start], tmp, MPI_INT, i, tag, MPI_COMM_WORLD);
		}
	} else {
		tag = menum +22;
		MPI_Recv(xloc, nloc, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
	}

	/* ************************************************************************ */
	// CALCOLO DELLA SOMMA PARZIALE

	if (scelta != 4) {
		printf("Applicazione della strategia %d.\n", scelta);
	}

	if (time_calc == 1) {
		MPI_Barrier(MPI_COMM_WORLD);
		t0 = MPI_Wtime();
	}

	sum = 0.0;
	for(int i=0; i < nloc; i++) {
		sum = sum + xloc[i];
	}

	/* ************************************************************************ */
	// SELEZIONE DELLA STRATEGIA

  	switch (scelta) {
		case 1: // Applicazione della strategia 1.
		{
			if (menum == 0) {
				for(int i=1; i < nproc; i++) {
					tag = i +80;
					MPI_Recv(&sum_parz, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &status);
					sum = sum + sum_parz;
				}
			} else {
				tag = menum +80;
				MPI_Recv(&sum, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status); //ci mancava status
			}

			break;
		}
		case 2: // Applicazione della strategia 2.
		{
			for(int i=0; i < log2(nproc); i++) { // passi di comunicazione
				if ((menum % pow(2.0, i)) == 0) { // chi partecipa alla comunicazione -- pow() vuole i double non gli int
					if ((menum % pow(2.0, i+1)) == 0) { // chi riceve le comunicazioni -- pow() vuole i double non gli int
						// ricevi da menum+2^i
					} else {
						// spedisci a menum-2^i
					}
				}
			}

			printf("\nProcesso n.%d\n", menum);
			printf("\nLa somma totale e' %f\n", sum);

			break;
		}
		case 3: // Applicazione della strategia 3.
		{
			for(int i=0; i < log2(nproc); i++) { // passi di comunicazione
				// tutti i processi partecipano ad ogni passo
				if ((menum % pow(2.0, i+1)) < pow(2.0, i)) { // si decide solo a chi si invia e da chi si riceve -- pow() vuole i double non gli int
					// ricevi da menum+2^i
					// spedisci a menum+2^i
				} else {
					// ricevi da menum-2^i
					// spedisci a menum-2^i
				}
			}

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
	// STAMPA DELL'OUTPUT

	if (time_calc == 1) {
		t1 = MPI_Wtime();
		time = t1 - t0;
		MPI_Reduce(&time, &timetot, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
		if (menum == 0) {
			printf("\nApplicazione della strategia %d terminata in %esec\n", scelta, timetot);
		}
	}

	switch (scelta) {
		case 1:
		{
			if (menum == 0) {
				printf("\nLa somma totale e' %f\n", sum);
			}
			break;
		}
		case 2:
		case 3:
		{
			printf("\nProcesso n.%d\n", menum);
			printf("\nLa somma totale e' %f\n", sum);
		}
		case 4:
		{

		}
		default:
			break;
	}

  	/* ************************************************************************ */

	printf("\nEsecuzione terminata.\n");
	return 0;
}

/* **************************************************************************** */

/* RIFERIMENTI

https://stackoverflow.com/questions/9748393/how-can-i-get-argv-as-int

*/
