#include <omp.h>
#include <stdio.h>

int main(){
    int i;
    int righe, colonne, threads;

    double **matrice = (double**) calloc(righe, sizeof(double*));
    double *vettore = (double*) calloc(colonne, sizeof(double*));
    double *prodotto = (double*)calloc(righe, sizeof(double));  
    
    //controllare che gli argomenti siano corretti

    //lettura degli argomenti
    
    /* Imposta il numero di threads da usare*/
    omp_set_num_threads(threads);
    
    //prendere i tempi

    prodotto = mat_vet(righe, colonne, matrice, vettore);
    
    //valutare i tempi

    //stampa il prodotto
    for(i = 0; i < righe; i++)
        printf(" [%lf] ", prodotto[i]);
    printf("\n");

    return 0;
}

double * mat_vet(int righe, int colonne, double **matrice, double *vettore){
    int i = 0, j=0;
    double *risultato = (double*)calloc(righe, sizeof(double));
    if (risultato){
        #pragma omp parallel for default(none) shared(righe, colonne, matrice, vettore, risultato) private (i,j)
        for(i=0; i<righe; i++){
            for(j=0; j<colonne; j++){
                risultato[i] = risultato[i] + matrice[i][j]*vettore[j];
            }
        }
    }
    return risultato;
}