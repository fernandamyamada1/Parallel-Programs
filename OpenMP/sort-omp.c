#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <sys/time.h>
#include <limits.h>
#include<omp.h>

#define SEED 1
#define N 8
#define NUM_THREADS 4




static int cmpstringp(const void *p1, const void *p2) {
  return *((int *)p1) - *((int *)p2);
}


int main(int argc, char *argv[])
{
    int *A ;
    int *B;
    
    int ratio=N/NUM_THREADS;
    A= (int *)malloc (N * sizeof(int));
    B= (int *)malloc (N * sizeof(int));
  int i;
  srand(SEED);

  
    for (i=0; i < N; i++) {
    
        A[i] = rand();
        B[i] = 0;
    }
    for (i=0; i < N; i++){
        printf("%d ", A[i]);
        printf("\n");
    }

  printf("Starting sorting.\n");
  struct timeval t1;
  gettimeofday(&t1, 0);

  int *a,*b,*c,*d;
    
#pragma omp parallel for
    for(i=0;i<NUM_THREADS;i++){
        int pos=i*ratio;
        int *p=&A[pos];
        qsort(p, ratio,sizeof(int), cmpstringp);
        
    }
    
    a=&A[0];
    b=&A[ratio];
    c=&A[ratio*2];
    d=&A[ratio*3];
    
    int counta=0;
    int countb=0;
    int countc=0;
    int countd=0;
    
    int min=INT_MAX;
    char pointer=' ';
    
    
    for(i=0;i<N;i++){
        if (*a<min && counta<ratio){
            min=*a;
            pointer='a';
            counta++;
        }
        if(*b<min && countb<ratio){
            min=*b;
            pointer='b';
            countb++;
        }
        if(*c<min && countc<ratio){
            min=*c;
            pointer='c';
            countc++;
        }
        if (*d<min && countd<ratio){
            min=*d;
            pointer='d';
            countd++;
        }
        B[i]=min;
        min=INT_MAX;
        switch (pointer){
                case 'a':
                a++;
                break;
                case 'b':
                b++;
                break;
                case 'c':
                c++;
                break;
                case 'd':
                d++;
                break;
        }
        pointer='o';
    }
    

  struct timeval t2;
  gettimeofday(&t2, 0);
  printf("time=%8.2f\n", (t2.tv_sec*1000. + t2.tv_usec/1000.) - (t1.tv_sec*1000. + t1.tv_usec/1000.));
  printf("Finished sorting.\n");
    
    
    for (i=0; i < N-1; i++){
      printf("%d ", B[i]);
    assert(B[i+1] >= B[i]);
    }
    
 
}
