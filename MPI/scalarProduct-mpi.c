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
    
    int *vetor1 = (int *) malloc ( sizeof(int) * nElemProc );
    int *vetor2 = (int *) malloc ( sizeof(int) * nElemProc );            
        
    for (i=0; i < nElemProc; i++) {
        vetor1[i] = 1; 
        vetor2[i] = nElemProc * my_rank + i;
    }
    
    int soma=0;
    int somaT=0;
    for (i=0; i < nElemProc; i++)
        soma += vetor1[i] * vetor2[i];
    
    MPI_Send (&soma, 1,
              MPI_INT, 0, 0, MPI_COMM_WORLD);
    
    if(my_rank==0){
        for(i=0;i<p;i++){
            int num=0;
            MPI_Recv (&num, 1,
                      MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            somaT+=num;
        }
    }
    
    printf("[%d]: the resut is  %d.\n", my_rank, soma);
    if(my_rank==0){
        printf("Final result: %d.\n", somaT);
    }
    
    free (vetor1);
    free (vetor2);
    
    MPI_Finalize();
}
