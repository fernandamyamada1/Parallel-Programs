#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>
#include<pthread.h>
int N = 200;

struct matrizes{
    long **M1, **M2, **M3;
    int tid;
};
void *matmul(void *param){
    struct matrizes *mat= (struct matrizes *)param;
    long **M1=mat->M1;
    long **M2=mat->M2;
    long **M3=mat->M3;
    int tid =mat->tid;
    int i,j;
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            M3[i][tid]=M1[i][j]*M2[j][tid];
        }
    }
    pthread_exit(NULL);
    
}

int main(int argc, char **argv) {
   
    struct timeval tv;
    gettimeofday(&tv, 0);
    long t1 = tv.tv_sec*1000 + tv.tv_usec/1000;

    long **a, **b, **c;

    struct matrizes *tmatrizes = (struct matrizes *)malloc((N) * sizeof(struct matrizes));
    pthread_t threads[N];

    if (argc == 2) {
      N = atoi (argv[1]);
      assert (N > 0);
    }

    int i,j,k,mul=2;
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
    

    gettimeofday(&tv, 0);
    long t2 = tv.tv_sec*1000 + tv.tv_usec/1000;
    printf ("Matrix generation finished %ldms.\n", t2-t1);
    for(i=0;i<N;i++){
        tmatrizes[i].M1=a;
        tmatrizes[i].M2=b;
        tmatrizes[i].M3=c;
        tmatrizes[i].tid=i;
        pthread_create(&threads[i], NULL, matmul, tmatrizes+i);
    }
    for(i=0;i<N;i++){
        pthread_join(threads[i], NULL);
    }
    

    
    gettimeofday(&tv, 0);
    long t3 = tv.tv_sec*1000 + tv.tv_usec/1000;
    printf ("Multiplication finished. %ldms.\n", t3-t2);

    gettimeofday(&tv, 0);
    long t4 = tv.tv_sec*1000 + tv.tv_usec/1000;
    printf ("Test finished. %ldms.\n", t4-t3);
}
