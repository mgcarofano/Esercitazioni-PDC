/*

	constants.c
	di Mario Gabriele Carofano
	e Francesco Noviello

*/

/* **************************************************************************** */
// CODICI DI ERRORE

/*
	NOT_ENOUGH_ARGS_ERROR: int
	Si utilizza per segnalare all'utente che l'esecuzione del programma
	e' stata terminata per mancanza di un numero sufficiente di argomenti.
*/

#define NOT_ENOUGH_ARGS_ERROR 1

/*
	EMPTY_ARG_ERROR: int
	Si utilizza per segnalare all'utente che l'esecuzione del programma
	e' stata terminata siccome un argomento dato in input e' vuoto.
*/

#define EMPTY_ARG_ERROR 2

/*
	INPUT_ARG_ERROR: int
	Si utilizza per segnalare all'utente che l'esecuzione del
    programma e' stata terminata a causa di un errata lettura di un
    argomento dato in input.
*/

#define INPUT_ARG_ERROR 3

/*
	NOT_INT_ARG_ERROR: int
	Si utilizza per segnalare all'utente che l'esecuzione del
    programma e' stata terminata poiche' un argomento dato in input
    non e' rappresentabile come intero.
*/

#define NOT_INT_ARG_ERROR 4

/*
	INPUT_LINE_ERROR: int
	Si utilizza per segnalare all'utente che l'esecuzione del
    programma e' stata terminata a causa di un errata lettura
    dello 'standard input stream' (stdin).
*/

#define INPUT_LINE_ERROR 5

/*
	NOT_REAL_NUMBER_ERROR: int
	Si utilizza per segnalare all'utente che l'esecuzione del
    programma e' stata terminata siccome non ha inserito un
    numero reale nel formato corretto.
*/

#define NOT_REAL_NUMBER_ERROR 6

/*
	NOT_NATURAL_NUMBER_ERROR: int
	Si utilizza per segnalare all'utente che l'esecuzione del
    programma e' stata terminata siccome non ha inserito un
    numero naturale nel formato corretto.
*/

#define NOT_NATURAL_NUMBER_ERROR 7

/*
	NOT_IN_RANGE_ERROR: int
	Si utilizza per segnalare all'utente che l'esecuzione del
    programma e' stata terminata siccome non ha inserito un
    numero compreso in un certo intervallo.
*/

#define NOT_IN_RANGE_ERROR 8

/*
	FILE_OPENING_ERROR: int
	Si utilizza per segnalare all'utente che l'esecuzione del
    programma e' stata terminata a causa di un errore durante
    l'apertura di un file.
*/

#define FILE_OPENING_ERROR 9

/*
	OVERFLOW_ERROR: int
	Si utilizza per segnalare all'utente che l'esecuzione del
    programma e' stata terminata siccome il file .csv letto in
	input contiene dei valori float con una maggior precisione
	di quella descritta dalla costante CSV_FIELD_PRECISION.
*/

#define OVERFLOW_ERROR 10

/* **************************************************************************** */
// ENUMERAZIONI E COSTANTI

/*
	Si utilizza per applicare l'algoritmo del calcolo del prodotto
	matrice-vettore in sequenziale quando si esegue un caso di
	test su un solo processore.
*/

#define NO_STRATEGY 0

/*
    Le seguenti 5 costanti sono valori interi che rappresentano
    le possibili scelte che può inserire l'utente nel menu' principale
    dell'applicazione.
*/

#define TESTING_SUITE 1
#define EXIT_APPLICATION 2

/*
    Si utilizza per eseguire l'algoritmo del calcolo del prodotto
	matrice-vettore utilizzando i valori degli operandi scelti
	dall'utente oppure generati in modo casuale.
*/

#define NO_TEST 0

/*
    Le seguenti 5 costanti sono valori interi che rappresentano
    le possibili scelte che può inserire l'utente nel menu'
    della suite di test dell'applicazione.
*/

#define MULTIPLICATION_ONE_TEST 1
#define MULTIPLICATION_SINGLE_NUMBER_TEST 2
#define MULTIPLICATION_CSV_TEST 3
#define MULTIPLICATION_EIGENVECTOR_TEST 4
#define EXIT_TEST 5

/*
    OP_MAX_QUANTITY: int
    Si utilizza per specificare il massimo numero di operandi
	che l'utente può inserire manualmente.
*/

#define OP_MAX_QUANTITY 20

/*
	NO_TIME_CALC: int
	OK_TIME_CALC: int
	Si utilizza per personalizzare il flusso di esecuzione del programma
	in quei controlli dove si sceglie se effettuare (OK_TIME_CALC)
	o meno (NO_TIME_CALC) il calcolo dei tempi di esecuzione.
*/

#define NO_TIME_CALC 0
#define OK_TIME_CALC 1

/*
	TIME_PRECISION: double
	Si utilizza per modificare la sensibilità dei tempi di esecuzione
	recuperati durante l'esecuzione del programma. In questo caso, il
	valore 1000000.0 indica che il tempo sarà misurato in secondi.
*/

#define TIME_PRECISION 1000000.0

/*
	NOME_PROVA: char*
	Si utilizza nella funzione createPBS() per personalizzare il
	file .pbs con il nome della prova da consegnare.
*/

#define NOME_PROVA "prova2"

/*
	NODE_NUMBER: char*
	Si utilizza nella funzione createPBS() per personalizzare il
	file .pbs con il numero di nodi con i quali si vuole
	eseguire il programma.
*/

#define NODE_NUMBER "1"

/*
	NODE_PROCESS: char*
	Si utilizza nella funzione createPBS() per personalizzare il
	file .pbs con il numero di processori occupati da ogni nodo
	per l'esecuzione del programma.
*/

#define NODE_PROCESS "8"

/*
	MKDIR_PATH: char*
	Si utilizza per indicare alla funzione system() il percorso
	sul cluster dell'eseguibile 'mkdir'.
*/

#define MKDIR_PATH "/bin/mkdir"

/*
	RM_PATH: char*
	Si utilizza per indicare alla funzione system() il percorso
	sul cluster dell'eseguibile 'rm'.
*/

#define RM_PATH "/bin/rm"

/*
	CSV_TIME_PATH: char*
	E' il path del file .csv nella quale sono memorizzate tutte le informazioni
	necessarie allo studio dei casi di test. In particolare, la sua struttura
	e' la seguente:
	-	test : int (indica il caso di test eseguito)
	-	n_proc : int (indica il numero di processori)
	-	rows : int (indica il numero di righe della matrice)
	-	cols : int (indica il numero di colonne della matrice e del vettore)
	-	t_tot : double (indica il tempo di esecuzione impiegato)
*/

#define CSV_TIME_PATH "../output"

/*
	OP_MAX_VALUE: int
	Si utilizza come limite massimo dei valori della matrice o
	del vettore quando si sceglie di utilizzare numeri reali casuali.
*/

#define OP_MAX_VALUE 100

/*
	CSV_FIELD_PRECISION: int
	Si utilizza come limite massimo di cifre decimali dei valori
	della matrice quando si sceglie la lettura da file .csv.
*/

#define CSV_FIELD_PRECISION 10
