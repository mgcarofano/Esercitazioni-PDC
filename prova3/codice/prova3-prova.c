/*

	prova3.c
	di Mario Gabriele Carofano
	e Francesco Noviello
    VERSIONE DI PROVA

*/

/* **************************************************************************** */
// LIBRERIE

#include "./libraries/auxfunc.h"
#include <mpi.h>

/* **************************************************************************** */
void random_matrix(int dim, double **mat);

int main(int argc, char **argv) {

	/* ************************************************************************ */
	// INIZIALIZZAZIONE DELL'AMBIENTE DI LAVORO

	int strategia = NO_STRATEGY, q_num = 0;
	int test = NO_TEST, time_calc = NO_TIME_CALC;
	
	int id_proc = 0, n_proc = 0, rest = 0;
	int q_loc = 0, tag = 0;
	int tmp = 0, i = 0, j = 0;
	int dim = 0;

	double t_start = 0.0, t_end = 0.0, t_tot = 0.0;

	double **mat1, **mat2;
	double **multiplication;

	MPI_Status status;

	srand(time(NULL));

	/* ************************************************************************ */
	// INIZIALIZZAZIONE DELL'AMBIENTE MPI

	/*
		--- int MPI_Init(int *argc, char ***argv) ---
		Definisce l'insieme dei processori attivati (contesto),
		assegnandone un identificativo
	*/

	MPI_Init(&argc, &argv);
	
	/*
		--- int MPI_Comm_rank(MPI_Comm comm, int *rank) ---
		Assegna ad ogni processore del communicator l'identificativo
		'id_proc' (sempre associato al contesto).
		MPI_COMM_WORLD indica il communicator a cui appartengono tutti
		i processori attivati (non puo' essere alterato).
	*/

	MPI_Comm_rank(MPI_COMM_WORLD, &id_proc);
	
	/*
		--- int MPI_Comm_size(MPI_Comm comm, int *size) ---
		Restituisce ad ogni processore di MPI_COMM_WORLD
		il numero di processori nel contesto.
	*/

	MPI_Comm_size(MPI_COMM_WORLD, &n_proc);

	/* ************************************************************************ */
	// LETTURA DEI DATI
	
	if (id_proc == 0) {
		printf("Inizio esecuzione.\n\n");

		/*
			Si affida al primo processore con id_proc == 0 il compito
			di leggere gli argomenti in input 'argv[]'.
		*/

		if (argc < 4) {
			printf("Errore nella lettura degli argomenti di input!\n\n");
			printf("Esecuzione terminata.\n");
			MPI_Finalize();
			exit(NOT_ENOUGH_ARGS_ERROR);
		}

		/*
			--- int argToInt(char *arg) ---
			Si utilizza la funzione 'argToInt' definita in 'auxfunc.h'
			per leggere gli argomenti contenuti nel vettore di 
			stringhe 'argv[]' e convertirli in valori interi.
		*/

		dim = argToInt(argv[1]);
		n_proc = argToInt(argv[2]);
		test = argToInt(argv[3]);
		time_calc = argToInt(argv[4]);

		/*
			Come richiesto dalle specifiche dell'algoritmo, 
            se la dimensione delle matrici quadrate non è multiplo della radice del numero dei processori
            l'esecuzione deve terminare.
		*/

		if (dim % sqrt(n_proc) != 0){
            printf("La dimensione delle matrici non è multiplo della radice del numero dei processori!\n\n");
			printf("Esecuzione terminata.\n");
			MPI_Finalize();
			exit(); // Creare codice di errore utile
		} else {
            //allocazione delle matrici e riempimento con numeri pseudo-random
            mat1 = (double**) calloc(rows, sizeof(double*));
            mat2 = (double**) calloc(rows, sizeof(double*));
            multiplication = (double**) calloc(rows, sizeof(double*));
            for (i = 0; i < dim; i++) {
                mat1[i] = (double*) calloc(cols, sizeof(double));
                mat2[i] = (double*) calloc(cols, sizeof(double));
                multiplication[i] = (double*) calloc(cols, sizeof(double));
            }
		
            //TODO: Creare griglia di processori in base a n_proc
        
        }
	}

	/*
		--- int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype,
							int root, MPI_Comm comm) ---
		Si utilizza questa funzione per inviare gli argomenti letti dal
		processore 0 (root) a tutti i processori del contesto, incluso se' stesso.
		Cioè, i valori di 'strategia', 'q_num' e 'time_calc' (interi di dimensione 1)
		saranno copiati in tutti i processori del communicator MPI_COMM_WORLD.
	*/

	MPI_Bcast(&dim, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&n_proc, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&test, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&time_calc, 1, MPI_INT, 0, MPI_COMM_WORLD);

	/* ************************************************************************ */
	// DISTRIBUZIONE DEGLI OPERANDI

	// Si distribuisce equamente il numero di operandi tra tutti i processori.
	sub_dim = dim / n_proc;

	/*
		--- void* calloc (size_t num, size_t size) ---
		Si utilizza questa funzione per allocare in memoria un vettore
		di 'num' elementi per memorizzare gli operandi locali.
		Ciascun elemento ha una dimensione del vettore ha una dimensione
		pari a 'size', e sara' inizializzato con tutti i suoi bit a 0.
	*/

    //si creano le sottomatrici
	mat1_loc = (double**) calloc(sub_dim, sizeof(double*));
    mat2_loc = (double**) calloc(sub_dim, sizeof(double*));
    for (i = 0; i < sub_dim; i++) {
        mat1_loc[i] = (double*) calloc(cols, sizeof(double));
        mat2_loc[i] = (double*) calloc(cols, sizeof(double));
    }

    //da qui in poi va riscritto tutto, o quasi, per il corretto prodotto matrice-matrice

	if (id_proc == 0) {

		/*
			Come prima, si affida al primo processore il compito di leggere
			gli operandi del prodotto (da argv[] se q_num <= 20) oppure di
			assegnarne un valore random.
		*/
	
		for (i=0; i < n_proc; i++) {

			/*
				Il seguente controllo serve per aggiornare il processore
				con id_proc == 0 sulla quantita' locale di operandi di
				tutti gli altri processori.
			*/

			op_tmp = (double *)calloc (tmp, sizeof(double));

			switch(test) {
				case NO_TEST: {
					if (q_num <= 20) {
						for (i=0; i < dim; i++) {
                            for (j=0; j < dim; j++){
                                //provvisoriamente le due matrici sono uguali quando inserite dall'utente
                                mat1[i][j] = argToDouble(argv[j+5]);
                                mat2[i][j] = argToDouble(argv[j+5]);
                            }
						}
					} else { //pseudorandom altrimenti
                        mat1 = random_matrix(dim, mat1);
                        mat2 = random_matrix(dim, mat2);
					}
					break;
				}
				case MAT_MAT_ONE_TEST: {
                    for (i=0; i < dim; i++) {
                        for (j=0; j < dim; j++){
                            //TUTTI 1
                            mat1[i][j] = 1;
                            mat2[i][j] = 1;
                        }
                    }
					break;
				}
				case MAT_MAT_SINGLE_NUMBER_TEST: {
						/*
							Ad ogni posizione del vettore degli operandi e'
							assegnato lo stesso valore reale, passato come
							argomento 'argv[5]' al programma.
						*/
                    for (i=0; i < dim; i++) {
                        for (j=0; j < dim; j++){
                            mat1[i][j] = argToDouble(argv[5]);
                            mat2[i][j] = argToDouble(argv[5]);
                        }
					}
					break;
				}
				case SUM_OPPOSITE_NUMBER_TEST: {
                    //TODO: Fare qualcosa
					break;
				}
				case GAUSS_TEST: {
					//TODO: Fare qualcosa
					break;
                }
			}

			if (i == 0) {
                // MODIFICARE... Assegnare alle sottomatrici la matrice...
				// Si assegnano gli operandi locali del processore con id_proc == 0
				for (j=0; j < tmp; j++) {
					op_loc[j] = op_tmp[j];
				}

			} else {

				/*
					Si distribuiscono equamente gli operandi rimanenti a tutti
					gli altri processori utilizzando la funzione MPI_Send().
				*/
                
                // il numero dei valori distribuiti da p0 a ogni processore è pari al rapporto tra n e la grandezza della griglia dei processori ...
                // ... quindi è pari a sub_dim
				tag = i + DISTRIBUTION_TAG;

				/*
					--- int MPI_Send(void *buf, int count, MPI_Datatype datatype,
										int dest, int tag, MPI_Comm comm) ---
					Il processo che esegue questa funzione spedisce i primi 'count'
					elementi di 'buf' di tipo 'datatype' al processo con
					identificativo 'dest'. In particolare, l'identificativo 'tag'
					individua univocamente l'invio nel contesto 'comm'.
				*/

				MPI_Send(&op_tmp[0], tmp, MPI_DOUBLE, i, tag, MPI_COMM_WORLD);
			}

			free(op_tmp);
		}
		
	} else {
		tag = id_proc + DISTRIBUTION_TAG;

		/*
			--- int MPI_Recv(void *buf, int count, MPI_Datatype datatype,
								int source, int tag, MPI_Comm comm,
								MPI_Status *status) ---
			Il processo che esegue questa funzione riceve i primi 'count'
			elementi di 'buf' di tipo 'datatype' dal processo con
			identificativo 'source'. In particolare, l'identificativo 'tag'
			individua univocamente la ricezione nel contesto 'comm', mentre
			'status' ne racchiude alcune informazioni.
		*/

		MPI_Recv(op_loc, q_loc, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &status);
	}

  	/* ************************************************************************ */
	// INIZIO DEL CALCOLO DEI TEMPI DI ESECUZIONE

	if (time_calc == OK_TIME_CALC) {

		/*
			--- int MPI_Barrier(MPI_Comm comm) ---
			Si utilizza questa funzione per restituire il controllo al
			chiamante solo dopo che tutti i processori del contesto 'comm'
			hanno effettuato la chiamata.
			Per calcolare correttamente i tempi di esecuzione in
			sicurezza, si aspetta che tutti i processori siano sincronizzati.
		*/

		MPI_Barrier(MPI_COMM_WORLD);

		/*
			--- double MPI_Wtime() ---
			Si utilizza per ottenere un valore di tempo in secondi
			rispetto ad un tempo arbitrario nel passato.
		*/

		t_start = MPI_Wtime();
	}

	/* ************************************************************************ */
	// CALCOLO DELLA SOMMA PARZIALE

	sum = 0.0;
	for(i=0; i < q_loc; i++) {
		sum = sum + op_loc[i];
	}

	/* ************************************************************************ */
	// APPLICAZIONE DELLA STRATEGIA

	switch (strategia) {
		case NO_STRATEGY: {
			break;
		}
		case FIRST_STRATEGY: // Applicazione della strategia 1.
		{
			if (id_proc == 0) {
				for(i=1; i < n_proc; i++) {
					tag = i + FIRST_STRATEGY_TAG;
					MPI_Recv(&sum_parz, 1, MPI_DOUBLE, i, tag, MPI_COMM_WORLD, &status);
					sum = sum + sum_parz;
				}
			} else {
				tag = id_proc + FIRST_STRATEGY_TAG;
				MPI_Send(&sum, 1, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
			}
			break;
		}
		case SECOND_STRATEGY: // Applicazione della strategia 2.
		{

			/*
				Nella 2a strategia, il numero di passi di comunicazione
				e' dato dal logaritmo in base 2 del numero di processori,
				gia' calcolato precedentemente.
			*/

			for(i=0; i < log_proc; i++) {

				/*
					Si calcola l'identificativo di chi deve partecipare
					alle comunicazioni all'i-esimo passo.
				*/

			 	pow_proc = pow(2, i);

			 	if ((id_proc % pow_proc) == 0) {

					/*
						Si calcola l'identificativo di chi deve ricevere
						dalle comunicazioni dell'i-esimo passo.
					*/

					pow_tmp = pow(2, i+1);

					if ((id_proc % pow_tmp) == 0) {

						/*
							Questi processori hanno il compito di ricevere la
							somma parziale da chi esegue la MPI_Send().
							In questo caso, sia 'tag' che 'tmp' si riferiscono
							allo stesso processore (chi invia).
						*/

						tmp = id_proc + pow(2, i);
						tag = tmp + SECOND_STRATEGY_TAG;
						MPI_Recv(&sum_parz, 1, MPI_DOUBLE, tmp, tag, MPI_COMM_WORLD, &status);
						sum = sum + sum_parz;

					} else {

						/*
							I processori rimanenti avranno il solo compito di
							inviare la propria somma parziale. In quest'altro
							caso, 'tag' e 'tmp' hanno valori diversi:
							-	tag: si riferisce a se' stesso (chi invia);
							-	tmp: si riferisce al ricevente (chi riceve).
						*/

						tmp = id_proc - pow(2, i);
						tag = id_proc + SECOND_STRATEGY_TAG;
						MPI_Send(&sum, 1, MPI_DOUBLE, tmp, tag, MPI_COMM_WORLD);

					}
				}
			}
			break;
		}
		case THIRD_STRATEGY: // Applicazione della strategia 3.
		{

			/*
				Nella 3a strategia, il numero di passi di comunicazione
				e' lo stesso della 2a strategia, ma al termine dell'esecuzione
				tutti i processori saranno a conoscenza della somma totale.
			*/

			for(i=0; i < log_proc; i++) {

				/*
					Rispetto alla 2a strategia, tutti i processori partecipano
					ad ogni passo di comunicazione. Operativamente, si rimuove
					il primo controllo '(id_proc % pow_tmp) == 0'.

					Quindi, tutti i processori hanno il compito di inviare le
					proprie somme parziali e, al contempo, ricevere le somme
					parziali dagli altri processori in modo tale che tutti
					calcolino localmente la somma totale.
					
					Operativamente, le operazioni di MPI_Send() e MPI_Recv()
					sono in comune a tutti i processori, cambia solo
					l'identificativo di chi ha inviato / riceve.
				*/

				pow_proc = pow(2, i+1);
				pow_tmp = pow(2, i);

				if ((id_proc % pow_proc) < pow_tmp) {
					tmp = id_proc + pow_tmp;
				} else {
					tmp = id_proc - pow_tmp;
				}

				tag = id_proc + THIRD_STRATEGY_TAG;
				MPI_Send(&sum, 1, MPI_DOUBLE, tmp, tag, MPI_COMM_WORLD);

				tag = tmp + THIRD_STRATEGY_TAG;
				MPI_Recv(&sum_parz, 1, MPI_DOUBLE, tmp, tag, MPI_COMM_WORLD, &status);
				sum = sum + sum_parz;
			}
			break;
		}
		default:
		{
			printf("Comando non riconosciuto!\n");
			break;
		}
	}

	/* ************************************************************************ */
	// SALVATAGGIO DEL CALCOLO DEI TEMPI DI ESECUZIONE

	if (time_calc == OK_TIME_CALC) {
		t_end = MPI_Wtime();

		// Si calcola la distanza di tempo tra l'istante iniziale e quello finale.
		t_loc = t_end - t_start;

		/*
			--- int MPI_Reduce(void *sendbuf, void *recvbuf, int count,
    				MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm) ---
			Si utilizza questa funzione per eseguire un'operazione collettiva
			su tutti i processori del contesto. In questo caso, si desidera 
			calcolare il massimo tempo tra tutti i tempi calcolati localmente.
		*/

		MPI_Reduce(&t_loc, &t_tot, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

		if (id_proc == 0) {
			printf("\nApplicazione della strategia %d terminata in %e sec\n", strategia, t_tot);
			writeTimeCSV(test, strategia, n_proc, q_num, t_tot);
		}
	}

  	/* ************************************************************************ */
	// STAMPA DELL'OUTPUT

	switch (strategia) {
		case NO_STRATEGY:
		case FIRST_STRATEGY:
		{
			if (id_proc == 0) {
				printf("\nLa somma totale e' %f\n", sum);
			}
			break;
		}
		case SECOND_STRATEGY:
		case THIRD_STRATEGY:
		{
			printf("\nProcesso n.%d\n", id_proc);
			printf("La somma totale e' %f\n", sum);
			break;
		}
		default:
			printf("Errore nella stampa dell'output!\n");
			break;
	}

  	/* ************************************************************************ */
	// TERMINAZIONE DELL'ESECUZIONE

	/*
		Attendiamo che tutti i processori abbiano portato a termine
		correttamente il loro carico di lavoro.
	*/

	MPI_Barrier(MPI_COMM_WORLD);

	// Al termine dell'esecuzione, si libera lo spazio allocato in memoria.
	free(op_loc);

	if (id_proc == 0) {
		printf("\nEsecuzione terminata.\n");
	}

	/*
		--- int MPI_Finalize() ---
		Determina la fine del programma MPI.
		Da questo punto in poi non e' possibile richiamare altre funzioni MPI.
	*/

	MPI_Finalize();
	return 0;
}

void random_matrix(int dim, double **mat){
    for (i = 0; i < dim; i++) {
        for (j = 0; j < dim; j++) {
            double_rand = (double)rand();
            int_rand = (int)rand();

            // Si genera un numero casuale reale compreso tra 0 e 100
            mat[i][j] = (double_rand / RAND_MAX) * OP_MAX_VALUE;

            // Si ha il 33% di possibilita che mat[i][j] < 0
            if (int_rand % 3 == 0) {
                mat[i][j] = mat[i][j] * (-1);
            }
        }
    }
}



/* **************************************************************************** */
/* RIFERIMENTI

https://stackoverflow.com/questions/9748393/how-can-i-get-argv-as-int

https://www.javatpoint.com/random-function-in-c
https://www.securitronlinux.com/bejiitaswrath/a-nice-example-of-c-programming-getting-a-random-number-in-milliseconds/?utm_content=cmp-true

https://www.educative.io/answers/how-to-check-if-a-number-is-a-power-of-2-in-cpp

http://www.vdepetris.it/t10/Text10.htm

*/
