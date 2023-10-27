#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char **argv) {

    double sumtot = 0;
    int n = 16, i = 0;
    double a[16] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    #pragma omp parallel for reduction (+=sumtot){
        for (i=0; i<n; i++){
            sumtot = sumtot + a[i];
        }
    };

    printf("La somma totale e': %f\n", sumtot);
    return 0;
}