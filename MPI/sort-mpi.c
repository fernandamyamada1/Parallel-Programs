    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
    #include <assert.h>
    #include <mpi.h>
    #include <limits.h>
    #include<sys/time.h>

    #define SEED 1
    #define N 16000000

    static int cmpstringp(const void *p1, const void *p2) {
    return *((int *)p1) - *((int *)p2);
    }

    int main(int argc, char *argv[])
    {

    int i, t, my_rank, p;
    MPI_Status status;

    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size (MPI_COMM_WORLD, &p);
        int nElemProc=N/p;

    srand(SEED);

    
    int *A = (int *)malloc (N * sizeof(int));
    int *AP= (int *)malloc (nElemProc * sizeof(int));
    if(my_rank==0){
    for (i=0; i < N; i++) 
    A[i] = rand();
    }

    printf("Starting sorting.\n");
    struct timeval t1;
    gettimeofday(&t1, 0);
    MPI_Scatter (A, nElemProc, MPI_INT,AP, nElemProc, MPI_INT,
                 0, MPI_COMM_WORLD );

    qsort(AP, nElemProc, sizeof(int), cmpstringp);
        MPI_Gather(AP, nElemProc, MPI_INT,
                   A, nElemProc, MPI_INT,
                   0, MPI_COMM_WORLD);
        int pos[p], min, min_t;
        for (t=0; t<p; t++)
            pos[t]=t*N/p;
        int *B = (int *)malloc (N * sizeof(int));
        if(my_rank==0){
            for (i=0; i < N; i++) {
                min = INT_MAX;
                for (t=0; t<p; t++)
                    if (pos[t] >= 0 &&  min > A[pos[t]]) {
                        min_t = t;
                        min = A[pos[t]];
                    }
                B[i] = A[pos[min_t]];
                pos[min_t]++;
                if (pos[min_t] == (min_t+1)*N/p)
                    pos[min_t] = -2*N;    
            }
        }

    struct timeval t2;
    gettimeofday(&t2, 0);
    printf("time=%8.2f\n", (t2.tv_sec*1000. + t2.tv_usec/1000.) - (t1.tv_sec*1000. + t1.tv_usec/1000.));
    printf("Finished sorting.\n");

    
        if(my_rank==0){
    for (i=0; i < N-1; i++) 
    assert(B[i+1] >= B[i]);
        }
        MPI_Finalize();
    }
