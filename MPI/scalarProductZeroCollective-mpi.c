#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main (int argc, char *argv[]) {
    int i, my_rank, p;
    int nElementos = 1000;
    MPI_Status status;
    
    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size (MPI_COMM_WORLD, &p);
    
    int nElemProc = nElementos / p;
    
    int *vetor1, *vetor2;
    int *vetor1P, *vetor2P;
    

        
        // Gera vetores originais
        vetor1 = (int *) malloc ( sizeof(int) * nElementos );
        vetor2 = (int *) malloc ( sizeof(int) * nElementos );
        vetor1P = (int *) malloc ( sizeof(int) * nElemProc );
        vetor2P = (int *) malloc ( sizeof(int) * nElemProc );
    
    if(my_rank==0){
        for (i=0; i < nElementos; i++) {
            vetor1[i] = i;
            vetor2[i] = 2*i;
        }
    }
    
    MPI_Scatter (vetor1, nElemProc, MPI_INT,
                 vetor1P, nElemProc, MPI_INT,
                 0, MPI_COMM_WORLD );
    MPI_Scatter (vetor2, nElemProc, MPI_INT,
                 vetor2P, nElemProc, MPI_INT,
                 0, MPI_COMM_WORLD );
    
    int soma=0;
    int somaT=0;
    for (i=0; i < nElemProc; i++)
        soma += vetor1P[i] * vetor2P[i];

    MPI_Reduce(&soma, &somaT,
               1, MPI_INT,
               MPI_SUM, 0, MPI_COMM_WORLD);
    
    printf("[%d]: The result is %d.\n", my_rank, soma);
    if(my_rank==0){
        printf("Final result: %d.\n", somaT);
    }
    
    free (vetor1);
    free (vetor2);
    
    MPI_Finalize();
}
