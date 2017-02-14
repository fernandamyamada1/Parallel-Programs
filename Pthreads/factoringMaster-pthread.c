#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#define N_ESCRAVOS 4

int proximo = -1, i = 0;
int nFatoracoes = 100;
pthread_mutex_t mutex;
pthread_cond_t condM;
pthread_cond_t condE;

struct strDivisores {
  int *lista;
  int tamanho;
  int nDivisores;
};

struct strDivisores encontraDivisores (int n) {

  //int nOriginal = n;
  struct strDivisores div;
  div.tamanho    = (int) ceilf ( log2f( (float)n ) );   
  div.lista      = (int *) malloc ( div.tamanho * sizeof(int) );
  div.nDivisores = 0; 

  int i;
  for (i=2; i <= n; i++) {
    if ( n % i == 0 ) { 
      while ( n % i == 0 && n != 1 ) {
	div.lista[ div.nDivisores ] = i;
	n = n/i;
      }
      div.nDivisores++;
    }
  }

  return div;
}


void *mestre(void *param) {
    
    int nMin =  100000000; // 10^8
    int nMax = 1000000000; // 10^9
    int i;
    
    for (i=0; i<nFatoracoes; i++) {
        pthread_mutex_lock(&mutex);
        proximo = (int) floor ( (double) rand() * (nMax - nMin) / RAND_MAX )  + nMin;
        pthread_cond_signal(&condE);
        pthread_cond_wait(&condM, &mutex);
        pthread_mutex_unlock(&mutex);
    }

}

void *escravo(void *param) {
    
    int k;
    
    while (1) {
        
        pthread_mutex_lock(&mutex);
        if (i >= nFatoracoes) {
            pthread_mutex_unlock(&mutex);
            pthread_exit(0);
        }
        int i_local = ++i;
        if(proximo==-1)
            pthread_cond_wait(&condE, &mutex);
        
        
        int n = proximo;
        proximo = -1;
        pthread_cond_signal(&condM);
        pthread_mutex_unlock(&mutex);
        
        struct strDivisores div = encontraDivisores (n);
        
        printf("[%3d] %10d: ", i_local,n);
        for (k=0; k<div.nDivisores; k++)
            printf("%10d ", div.lista[k]);
        printf("\n");    
        
        free (div.lista);
    }
    
}

int main (int argc, char *argv[])
{

  srand(1);

  int i;

  pthread_t mestre_t;
  pthread_t escravo_t[N_ESCRAVOS];
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condE, NULL);
    pthread_cond_init(&condM, NULL);

  pthread_create(&mestre_t, NULL, mestre, NULL);

  for (i=0; i<N_ESCRAVOS; i++)
      pthread_create(&escravo_t[i], NULL, escravo, NULL);

  pthread_exit(0);
    return 0;
}

