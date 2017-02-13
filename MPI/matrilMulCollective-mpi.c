#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>


#define N 800

int main(int argc, char **argv) {

    struct timeval tv;
    gettimeofday(&tv, 0);
    long t1 = tv.tv_sec*1000 + tv.tv_usec/1000;

    int my_rank, p;
    int nElementos = 8;
    MPI_Status status;
    
    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size (MPI_COMM_WORLD, &p);

    gettimeofday(&tv, 0);
    long t2 = tv.tv_sec*1000 + tv.tv_usec/1000;
    if (my_rank==0)
      printf("Init = %ld\n", t2-t1);

    long *a, *b, *c;
    long *aP, *cP;

    int i,j,k,mul=2;
    int col_sum = N * (N-1) / 2;
    int Np = N / p;

    if (my_rank == 0) {
      a = (long *)malloc (N * N * sizeof(long));
      b = (long *)malloc (N * N * sizeof(long));
      c = (long *)malloc (N * N * sizeof(long));
        aP = (long *)malloc (N*Np  * sizeof(long));
        cP = (long *)malloc (N*Np  * sizeof(long));
      for (i=0; i<N; i++)
        for (j=0; j<N; j++) {
  	  a[i*N+j] = i*mul;
	  b[i*N+j] = i;
        } 
    }
    else {
        a = (long *)malloc (N * N * sizeof(long));
        b = (long *)malloc (N * N * sizeof(long));
        c = (long *)malloc (N * N * sizeof(long));
        aP = (long *)malloc (N*Np  * sizeof(long));
        cP = (long *)malloc (N*Np  * sizeof(long));
    }
 
    for (i=0; i<Np; i++)
      for (j=0; j<N; j++)
        c[i*N+j] = 0;

    gettimeofday(&tv, 0);
    long t3 = tv.tv_sec*1000 + tv.tv_usec/1000;
    if (my_rank==0)
      printf("Create = %ld\n", t3-t2);

    
    MPI_Bcast(b, N*N, MPI_LONG, 0, MPI_COMM_WORLD);
    MPI_Scatter (a, N*Np, MPI_LONG,
                 aP, N*Np, MPI_LONG,
                 0, MPI_COMM_WORLD);
    if (my_rank == 0) {
     for (i=1; i<p; i++) {
           MPI_Send(c + i*N*Np, N*Np, MPI_LONG, i, 0, MPI_COMM_WORLD);
      }
    }
    else {
        MPI_Recv(cP, N*Np, MPI_LONG, 0, 0, MPI_COMM_WORLD, &status);
    }

    gettimeofday(&tv, 0);
    long t4 = tv.tv_sec*1000 + tv.tv_usec/1000;
    if (my_rank==0)
      printf("Sending = %ld\n", t4-t3);

    for (i=0; i<Np; i++)
      for (j=0; j<N; j++)
	for (k=0; k<N; k++)
	  cP[i*N+j] += aP[i*N+k] * b[k*N+j];

    gettimeofday(&tv, 0);
    long t6 = tv.tv_sec*1000 + tv.tv_usec/1000;
    if (my_rank==0)
      printf("Matrixmul = %ld\n", t6-t4);

    MPI_Gather(cP, N*Np, MPI_LONG,
               c, N*Np, MPI_LONG,
               0, MPI_COMM_WORLD);

   gettimeofday(&tv, 0);
    long t7 = tv.tv_sec*1000 + tv.tv_usec/1000;
    if (my_rank==0)
      printf("Receiving = %ld\n", t7-t6);

   if (my_rank == 0) {
     for (i=0; i<N; i++)
       for (j=0; j<N; j++)
	 assert ( c[i*N+j] == i*mul * col_sum);        
   }

    MPI_Finalize ();    
}
