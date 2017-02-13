#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <mpi.h>
#include<sys/time.h>

#define N 800

int main(int argc, char **argv) {

    struct timeval tv;
    gettimeofday(&tv, 0);
    long t1 = tv.tv_sec*1000 + tv.tv_usec/1000;

    int npes, myrank, valor=0, target;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &npes);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    long **a, **b, **c;

    int i,j,k,mul=2;
    int col_sum = N * (N-1) / 2;
    int Np = N/npes;

    if (myrank == 0) {
      a = (long **)malloc (N * sizeof(long *));
      b = (long **)malloc (N * sizeof(long *));
      c = (long **)malloc (N * sizeof(long *));
      for (i=0; i<N; i++) {
        a[i] = (long *)malloc (N * sizeof(long));
        b[i] = (long *)malloc (N * sizeof(long));
        c[i] = (long *)malloc (N * sizeof(long));
      }
    }
    else {
      a = (long **)malloc (N/npes * sizeof(long *));
      b = (long **)malloc (N * sizeof(long *));
      c = (long **)malloc (N/npes * sizeof(long *));
      for (i=0; i<N/npes; i++) {
        a[i] = (long *)malloc (N * sizeof(long));
        c[i] = (long *)malloc (N * sizeof(long));
      }
      for (i=0; i<N; i++)
        b[i] = (long *)malloc (N * sizeof(long));
    }

    if (myrank==0) {
      for (i=0; i<N; i++)
        for (j=0; j<N; j++) {
          a[i][j] = i*mul;
  	  b[i][j] = i;
          c[i][j] = 0;
       }
   }

    gettimeofday(&tv, 0);
    long t2 = tv.tv_sec*1000 + tv.tv_usec/1000;
    printf("Create = %ld\n", t2-t1);

   if (myrank == 0) {
     for (i=1; i<npes; i++) 
       for (j=0; j<N; j++)
         MPI_Send(b[j], N, MPI_LONG, i, 0, MPI_COMM_WORLD);    

      for (j=Np; j<N; j++) {
         MPI_Send(a[j], N, MPI_LONG, j/Np, 0, MPI_COMM_WORLD);
         MPI_Send(c[j], N, MPI_LONG, j/Np, 0, MPI_COMM_WORLD);
      }
   }
   else {
     for (j=0; j<N; j++) 
         MPI_Recv(b[j], N, MPI_LONG, 0, 0, MPI_COMM_WORLD, &status);
     for (j=0; j<Np; j++) {
         MPI_Recv(a[j], N, MPI_LONG, 0, 0, MPI_COMM_WORLD, &status);
         MPI_Recv(c[j], N, MPI_LONG, 0, 0, MPI_COMM_WORLD, &status);
     }
   }

    for (i=0; i<Np; i++)
      for (j=0; j<N; j++)
	for (k=0; k<N; k++)
	  c[i][j] += a[i][k] * b[k][j];

    gettimeofday(&tv, 0);
    long t3 = tv.tv_sec*1000 + tv.tv_usec/1000;
    printf("Mult = %ld\n", t3-t2);

   if (myrank == 0) {
     for (i=0; i<N; i++)
       for (j=0; j<N; j++)
	 assert ( c[i][j] == i*mul * col_sum);        
   }

   gettimeofday(&tv, 0);
   long t4 = tv.tv_sec*1000 + tv.tv_usec/1000;
   printf("Checking = %ld\n", t4-t3);

   MPI_Finalize();
}
