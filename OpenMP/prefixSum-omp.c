#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>
#define VECTOR_SIZE 10
#define NUM_THREADS 2

int main() {

  int i, j;

  int size = VECTOR_SIZE;
  
  double *elements = (double *)malloc( size * sizeof(double));
    
    for (i=0; i<size; i++){
      elements[i] = i;
        printf("[%d]=%f\n ", i, elements[i]);
    }
    
    
  struct timeval t1;
  gettimeofday(&t1, 0);

    int range=size/NUM_THREADS;
#pragma omp parallel for private(i)
    for(j=0;j<NUM_THREADS;j++){
    
        for(i=1;i<range;i++){
            int n=omp_get_thread_num();
            elements[n*range+i]+=elements[n*range+i-1];
        }
    }
    
    
#pragma omp parallel for private(j)
    for(i=1;i<size;i++){
        if(i%range==0){
            for(j=i;j<size;j++){
                elements[j]+=elements[i-1];
            }
        }
    }


  struct timeval t2;
  gettimeofday(&t2, 0);

  printf("time=%8.2f\n", (t2.tv_sec*1000. + t2.tv_usec/1000.) - (t1.tv_sec*1000. + t1.tv_usec/1000.));
  printf("middle=%.5lf last=%.5lf\n", elements[size/2], elements[size-1]);
    
    for (i=0; i<size; i++){
        printf("[%d]=%f\n ", i, elements[i]);
    }

  return 0;
}
