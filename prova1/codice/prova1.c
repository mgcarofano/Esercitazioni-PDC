/*

	prova1.c
	di Mario Gabriele Carofano
	e Francesco Noviello

*/

/* **************************************************************************** */
// LIBRERIE

#include "./libraries/auxfunc.h"

/* **************************************************************************** */

int main(int argc, char **argv) {

	/* ************************************************************************ */
	// DEFINIZIONE DELLE VARIABILI

	int scelta = 0, q_num = 0;
	int test = NO_TEST, time_calc = NO_TIME_CALC;
	
	int id_proc = 0, n_proc = 0, rest = 0;
	int q_loc = 0, tag = 1;
	int tmp = 0, id_op = 0;
	int i = 0, pow_proc = 0, pow_tmp = 0, log_proc = 0;

	double *op, *op_loc;
	double sum = 0.0, sum_parz = 0.0;

	double t_start = 0.0, t_end = 0.0;
	double t_loc = 0.0, t_tot = 0.0;

	int int_rand = 0;
	double double_rand = 0.0;
	time_t seed;

	MPI_Status status;

	/* ************************************************************************ */
	// INIZIALIZZAZIONE DI MPI

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
		i processori attivati (non può essere alterato).
	*/

	MPI_Comm_rank(MPI_COMM_WORLD, &id_proc);
	
	/*
		--- int MPI_Comm_size(MPI_Comm comm, int *size) ---
		Restituisce ad ogni processore di MPI_COMM_WORLD
		il numero di processori nel contesto.
	*/

	MPI_Comm_size(MPI_COMM_WORLD, &n_proc);

	/* ******************************************************************** */
	// LETTURA E DISTRIBUZIONE DEI DATI
	
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

		scelta = argToInt(argv[1]);
		q_num = argToInt(argv[2]);
		test = argToInt(argv[3]);
		time_calc = argToInt(argv[4]);

		/*
			Come richiesto dalle specifiche dell'algoritmo, se le strategie
			2 e 3 non sono applicabili (perche' il numero di processori
			non e' potenza di 2), allora si deve applicare la strategia 1.

			Per realizzare questo controllo, si calcola il log2() del numero
			di processori 'n_proc' e, quindi, si utilizzano le funzioni ceil()
			che, rispettivamente, restituiscono il tetto e la base.

			Se questi valori calcolati sono diversi, allora si e' in presenza
			di un numero di processori che non e' potenza di 2. Allora si
			deve applicare la strategia 1.
		*/

		if (n_proc > 1) {

			if (!(ceil(log2(n_proc)) == floor(log2(n_proc)))) {
				printf("Il numero di processori (%d) non e' potenza di 2.\n", n_proc);
				scelta = FIRST_STRATEGY;
			} else {
				log_proc = log2(n_proc);
			}

			printf("Applicazione della strategia %d.\n", scelta);

		} else {

			printf("Il numero di processori (%d) non e' sufficiente per applicare la strategia %d.\n", n_proc, scelta);
			scelta = NO_STRATEGY;
			printf("Calcolo della somma in sequenziale.\n");

		}

	}

	/*
		--- int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype,
							int root, MPI_Comm comm) ---
		Si utilizza questa funzione per inviare gli argomenti letti dal
		processore 0 (root) a tutti i processori del contesto, incluso se' stesso.
		Cioè, i valori di 'scelta', 'q_num' e 'time_calc' (interi di dimensione 1)
		saranno copiati in tutti i processori del communicator MPI_COMM_WORLD.
	*/

	MPI_Bcast(&scelta, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&q_num, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&test, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&time_calc, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&log_proc, 1, MPI_INT, 0, MPI_COMM_WORLD);

	// Si distribuisce equamente il numero di operandi tra tutti i processori.
	q_loc = q_num / n_proc;

	//	Gli operandi rimanenti sono distribuiti tra i primi 'rest' processori.
	rest = q_num % n_proc;
	if (id_proc < rest) {
		q_loc++;
	}

	/*
		--- void* calloc (size_t num, size_t size) ---
		Si utilizza questa funzione per allocare in memoria un vettore
		di 'num' elementi per memorizzare gli operandi locali.
		Ciascun elemento ha una dimensione del vettore ha una dimensione
		pari a 'size', e sarà inizializzato con tutti i suoi bit a 0.
	*/

	op_loc = (double *)calloc (q_loc, sizeof(double));

	if (id_proc == 0) {

		/*
			Come prima, si affida al primo processore il compito di leggere
			gli operandi della somma (da argv[] se q_num <= 20) oppure di
			assegnarne un valore random.
		*/

		op = (double *)calloc (q_num, sizeof(double));

		switch(test) {
			case NO_TEST: {
				if (q_num <= 20) {
					for (i=0; i < q_num; i++) {
						op[i] = argToDouble(argv[i+5]);
					}
				} else {

					srand((unsigned)time(&seed));

					for (i=0; i < q_num; i++) {
						double_rand = (double)rand();
						int_rand = (int)rand();

						// Si genera un numero casuale reale compreso tra 0 e 100
						op[i] = (double_rand / RAND_MAX) * OP_MAX_VALUE;

						// Si ha il 33% di possibilita che op[i] < 0
						if (int_rand % 3 == 0) {
							op[i] = op[i] * (-1);
						}
					}
				}
				break;
			}
			case SUM_ONE_TEST: {
				break;
			}
			case SUM_20_NUMBERS_TEST: {
				break;
			}
			case GAUSS_TEST: {
				break;
			}
			default:
				break;
		}

		// Si assegnano gli operandi locali del processore con id_proc == 0
		for (i=0; i < q_loc; i++) {
			op_loc[i] = op[i];
		}

		/*
			Si distribuiscono equamente gli operandi rimanenti a tutti
			gli altri processori utilizzando la funzione MPI_Send().
		*/

		tmp = q_loc;
		for (i=1; i < n_proc; i++) {
			id_op = id_op + tmp;
			tag = i + DISTRIBUTION_TAG;

			/*
				Il seguente controllo serve per aggiornare il processore
				con id_proc == 0 sulla quantita' locale di operandi di
				tutti gli altri processori.
			*/

			if (i == rest) {
				tmp--;
			}

			/*
				--- int MPI_Send(void *buf, int count, MPI_Datatype datatype,
									int dest, int tag, MPI_Comm comm) ---
				Il processo che esegue questa funzione spedisce i primi 'count'
				elementi di 'buf' di tipo 'datatype' al processo con
				identificativo 'dest'. In particolare, l'identificativo 'tag'
				individua univocamente l'invio nel contesto 'comm'.
			*/

			MPI_Send(&op[id_op], tmp, MPI_DOUBLE, i, tag, MPI_COMM_WORLD);
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
	// SELEZIONE DELLA STRATEGIA

  	switch (scelta) {
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
				Nella 2° strategia, il numero di passi di comunicazione
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
				Nella 3° strategia, il numero di passi di comunicazione
				e' lo stesso della 2° strategia, ma al termine dell'esecuzione
				tutti i processori saranno a conoscenza della somma totale.
			*/

			for(i=0; i < log_proc; i++) {

				/*
					Rispetto alla 2° strategia, tutti i processori partecipano
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
	// FINE DEL CALCOLO DEI TEMPI DI ESECUZIONE

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
			printf("\nApplicazione della strategia %d terminata in %e sec\n", scelta, t_tot);
		}
	}

  	/* ************************************************************************ */
	// STAMPA DELL'OUTPUT

	switch (scelta) {
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
		free(op);
		printf("\nEsecuzione terminata.\n");
	}

	/*
		--- int MPI_Finalize() ---
		Determina la fine del programma MPI.
		Da questo punto in poi non è possibile richiamare altre funzioni MPI.
	*/

	MPI_Finalize();
	return 0;
}

/* **************************************************************************** */
/* RIFERIMENTI

https://stackoverflow.com/questions/9748393/how-can-i-get-argv-as-int

https://www.javatpoint.com/random-function-in-c
https://www.securitronlinux.com/bejiitaswrath/a-nice-example-of-c-programming-getting-a-random-number-in-milliseconds/?utm_content=cmp-true

https://www.educative.io/answers/how-to-check-if-a-number-is-a-power-of-2-in-cpp

*/
