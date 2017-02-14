#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <assert.h>
#include<pthread.h>
#define NUM_THREADS 4
struct matrizes {
    long id;
    int N;
    long **M1, **M2, **M3;
    
};

void *matmul(void *param){
    struct matrizes *mat= (struct matrizes *)param;
    long id= mat->id;
    int N = mat->N;
    long **M1 = mat->M1;
    long **M2 = mat->M2;
    long **M3 = mat->M3;
    int i,j,k;
    int iStart=(mat->id/2)*(mat->N)/2;
    int jStart=(mat->id%2)*(mat->N)/2;
    int iEnd=(mat->id/2+1)*(mat->N)/2;
    int jEnd=(mat->id%2+1)*(mat->N)/2;
    
    for (i=iStart; i<iEnd; i++)
        for(j=jStart; j<jEnd; j++)
            for (k=0; k<N; k++)
                M3[i][j] += M1[i][k] * M2[k][j];
    
    
    
    
    pthread_exit(NULL);
    
    
    
}


int main(int argc, char **argv) {
    
    long **a, **b, **c;
    int N = 2;
    
    if (argc == 2) {
        N = atoi (argv[1]);
        assert (N > 0);
    }
    
    int i,j,k,mul=5;
    int col_sum = N * (N-1) / 2;
    
    a = (long **)malloc (N * sizeof(long *));
    b = (long **)malloc (N * sizeof(long *));
    c = (long **)malloc (N * sizeof(long *));
    for (i=0; i<N; i++) {
        a[i] = (long *)malloc (N * sizeof(long));
        b[i] = (long *)malloc (N * sizeof(long));
        c[i] = (long *)malloc (N * sizeof(long));
    }
    
    
    for (i=0; i<N; i++)
        for (j=0; j<N; j++) {
            a[i][j] = i*mul;
            b[i][j] = i;
            c[i][j] = 0;
        }
    
    printf ("Matrix generation finished.\n");
    
    struct matrizes *tmatrizes = (struct matrizes *)malloc((N) * sizeof(struct matrizes));
    pthread_t threads[N]; //usado para a referencia de threads
    
    
    long t;
    for(t=0; t<N; t++){
        tmatrizes[t].N=N;
        tmatrizes[t].M1=a;
        tmatrizes[t].M2=b;
        tmatrizes[t].M3=c;
        tmatrizes[t].id=t;
        printf("In main: creating thread %ld\n", t);
        pthread_create(&threads[t], NULL, matmul, tmatrizes+t);
    }
    
    for(t=0; t<N; t++)
        pthread_join(threads[t], NULL);
    
    
    printf ("Multiplication finished. %ld\n", c[1][1]);
    
}
