/*

	constants.c
	di Mario Gabriele Carofano
	e Francesco Noviello

*/

/* **************************************************************************** */
// CODICI DI ERRORE

/*
	NOT_ENOUGH_ARGS_ERROR: int
	Si utilizza per segnalare all'utente che l'esecuzione del
    programma è stata terminata per mancanza di un numero sufficiente di argomenti.
*/

#define NOT_ENOUGH_ARGS_ERROR 1

/*
	EMPTY_ARG_ERROR: int
	Si utilizza per segnalare all'utente che l'esecuzione del
    programma è stata terminata siccome un argomento dato in input e' vuoto.
*/

#define EMPTY_ARG_ERROR 2

/*
	INPUT_ARG_ERROR: int
	Si utilizza per segnalare all'utente che l'esecuzione del
    programma è stata terminata a causa di un errata lettura di un
    argomento dato in input.
*/

#define INPUT_ARG_ERROR 3

/*
	NOT_INT_ARG_ERROR: int
	Si utilizza per segnalare all'utente che l'esecuzione del
    programma è stata terminata poiche' un argomento dato in input
    non è rappresentabile come intero.
*/

#define NOT_INT_ARG_ERROR 4

/*
	INPUT_LINE_ERROR: int
	Si utilizza per segnalare all'utente che l'esecuzione del
    programma è stata terminata a causa di un errata lettura
    dello 'standard input stream' (stdin).
*/

#define INPUT_LINE_ERROR 5

/*
	NOT_REAL_NUMBER_ERROR: int
	Si utilizza per segnalare all'utente che l'esecuzione del
    programma è stata terminata siccome non ha inserito un
    numero reale correttamente.
*/

#define NOT_REAL_NUMBER_ERROR 6

/*
	NOT_NATURAL_NUMBER_ERROR: int
	Si utilizza per segnalare all'utente che l'esecuzione del
    programma è stata terminata siccome non ha inserito un
    numero naturale correttamente.
*/

#define NOT_NATURAL_NUMBER_ERROR 7

/*
	NOT_IN_RANGE_ERROR: int
	Si utilizza per segnalare all'utente che l'esecuzione del
    programma è stata terminata siccome non ha inserito un
    numero compreso in un certo intervallo.
*/

#define NOT_IN_RANGE_ERROR 8

/*
	NOT_ENOUGH_OPERANDS: int
	Si utilizza per segnalare all'utente che l'esecuzione del
    programma è stata terminata siccome non ha inserito un
    numero sufficiente di operandi per la somma.
*/

#define NOT_ENOUGH_OPERANDS 9

/*
	FILE_OPENING_ERROR: int
	Si utilizza per segnalare all'utente che l'esecuzione del
    programma è stata terminata a causa di un errore durante
    l'apertura di un determinato file.
*/

#define FILE_OPENING_ERROR 10

/* **************************************************************************** */
// ENUMERAZIONI E COSTANTI

/*
	Si utilizza per applicare l'algoritmo del calcolo della somma in
	sequenziale quando si esegue un caso di test su un solo processore.
*/

#define NO_STRATEGY 0

/*
    Le seguenti 5 costanti sono valori interi che rappresentano
    le possibili scelte che può inserire l'utente nel menu' principale
    dell'applicazione.
*/

#define FIRST_STRATEGY 1
#define SECOND_STRATEGY 2
#define THIRD_STRATEGY 3
#define TESTING_SUITE 4
#define EXIT_APPLICATION 5

/*
    Si utilizza per eseguire l'algoritmo del calcolo della somma utilizzando
	i valori degli operandi scelti dall'utente oppure generati in modo casuale.
*/

#define NO_TEST 0

/*
    Le seguenti 5 costanti sono valori interi che rappresentano
    le possibili scelte che può inserire l'utente nel menu'
    della suite di test dell'applicazione.
*/

#define SUM_ONE_TEST 1
#define SUM_SINGLE_NUMBER_TEST 2
#define SUM_OPPOSITE_NUMBER_TEST 3
#define GAUSS_TEST 4
#define EXIT_TEST 5

/*
    OP_MIN_EXP_TEST: int
    Si utilizza come esponente minimo per determinare la quantita' di
	operandi nella creazione dei casi di test.
*/

#define OP_MIN_EXP_TEST 4

/*
    OP_MAX_EXP_TEST: int
    Si utilizza come esponente massimo per determinare la quantita' di
	operandi nella creazione dei casi di test.
*/

#define OP_MAX_EXP_TEST 6

/*
    OP_MAX_QUANTITY: int
    Come richiesto dalle specifiche dell'algoritmo, il massimo numero
    di operandi che l'utente può inserire manualmente è 20.
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
	NOME_PROVA: char*
	Si utilizza nella funzione createPBS() per personalizzare il
	file .pbs con il nome della prova corrente.
*/

/*
	TIME_PRECISION: double
	
*/

#define TIME_PRECISION 1000000.0

#define NOME_PROVA "prova2"

/*
	NODE_NUMBER: char*
	Si utilizza nella funzione createPBS() per personalizzare il
	file .pbs con il numero di nodi con i quali si vuole
	eseguire il programma.
*/

#define NODE_NUMBER "8"

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
	È il path del file .csv nella quale sono memorizzate tutte le informazioni
	necessarie allo studio dei casi di test. In particolare, la sua struttura
	e' la seguente:
	-	test : int (indica il caso di test eseguito)
	-	strategia : int (indica la strategia scelta per l'esecuzione)
	-	n_proc : int (indica il numero di processori)
	-	q_num : int (indica il numero di operandi sommati)
	-	t_tot : double (indica il tempo di esecuzione impiegato)
*/

#define CSV_TIME_PATH "../output"

/*
	OP_MAX_VALUE: int
	Si utilizza come limite massimo del valore di un operando
	quando si sceglie di sommare numeri reali casuali.
*/

#define OP_MAX_VALUE 100

/*
	DISTRIBUTION_TAG: int
	Si utilizza per assegnare un identificativo unico alle
	comunicazioni di MPI durante la fase di distribuzione dei dati.
*/

#define DISTRIBUTION_TAG 25

/*
	FIRST_STRATEGY_TAG: int
	Si utilizza per assegnare un identificativo unico alle
	comunicazioni di MPI durante l'applicazione della 1a strategia.
*/

#define FIRST_STRATEGY_TAG 100

/*
	SECOND_STRATEGY_TAG: int
	Si utilizza per assegnare un identificativo unico alle
	comunicazioni di MPI durante l'applicazione della 2a strategia.
*/

#define SECOND_STRATEGY_TAG 200

/*
	THIRD_STRATEGY_TAG: int
	Si utilizza per assegnare un identificativo unico alle
	comunicazioni di MPI durante l'applicazione della 3a strategia.
*/

#define THIRD_STRATEGY_TAG 300
