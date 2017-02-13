#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

int main(int argc, char **argv) {
    long N = 20000;
   
    long *a = (long *)malloc (N * sizeof(long));
    
    long media = 0;
    long desvio=0;

    int i;
    for (i=0; i<N; i++)
	a[i] = i;
    
#pragma omp parallel
    {
    
#pragma  for reduction(+:media)
    for (i=0; i<N; i++)
 	  media += a[i];
    
    media=media/N;
    
#pragma for reduction(+:desvio)
    for(i=0;i<N;i++){
        desvio+=(a[i]-media)*(a[i]-media)/N;
    }
    desvio=sqrt(desvio);
    }
    printf ("Mean = %ld \n", media);
    printf ("Standard Deviation = %ld \n", desvio);
    
}
