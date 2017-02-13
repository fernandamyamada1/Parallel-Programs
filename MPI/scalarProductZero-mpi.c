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
    
    if (my_rank == 0) {
        
        vetor1 = (int *) malloc ( sizeof(int) * nElementos );
        vetor2 = (int *) malloc ( sizeof(int) * nElementos );            
        
        for (i=0; i < nElementos; i++) {
            vetor1[i] = 1;
            vetor2[i] = i; 
        }
    }
    else {

        vetor1 = (int *) malloc ( sizeof(int) * nElemProc );
        vetor2 = (int *) malloc ( sizeof(int) * nElemProc );    

    }

    
    if(my_rank==0){
        for(i=1;i<p;i++){
            MPI_Send (vetor1+nElemProc*(i-1), nElemProc,
                      MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send (vetor2+nElemProc*(i-1), nElemProc,
                      MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }
    else{
        MPI_Recv (vetor1, nElemProc,
                  MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv (vetor2, nElemProc,
              MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }
    
    int soma=0;
    if(my_rank!=0){
        for (i=0; i < nElemProc; i++){
        soma += vetor1[i] * vetor2[i];
        }
        MPI_Send (&soma, 1,
                  MPI_INT, 0, 1, MPI_COMM_WORLD);
    }
    
    else{
        int num=0;
        for(i=1;i<p;i++){
            MPI_Recv (&num, 1,
                      MPI_INT, i, 1, MPI_COMM_WORLD, &status);
            soma+=num;
        }
        
    
    }

    
    free (vetor1);
    free (vetor2);
    
    MPI_Finalize();
}
