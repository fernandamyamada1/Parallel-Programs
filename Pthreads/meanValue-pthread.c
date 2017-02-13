#include <stdio.h>
#include <stdlib.h>
#include<pthread.h>
#include <math.h>
#define NUM_THREADS 10
#define N 20

int tid=0;
int size=N/NUM_THREADS;
long media=0;

pthread_mutex_t mutex;



void *calcMedia(void *threadid){
    int  parcial=0;
    int i;
    long id = *((long *)threadid);
    for(i=(id)*size; i<(id+1)*size;i++)
        parcial+=i;
    printf("THREAD[%ld] calculated %d\n", id, parcial);
    pthread_mutex_lock(&mutex);
    media+=parcial;
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
    
}
int main(int argc, char **argv) {

    int i;
    long *id = (long *) malloc ( NUM_THREADS * sizeof (long));
   
    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&mutex, NULL);

    for(i=0;i<N;i++){
        id[i]=i;
        printf("[%d]= %ld \n", i, id[i]);
    }
    for(i=0;i<NUM_THREADS;i++){
        
        pthread_create(&threads[i], NULL, calcMedia,  (void *)(id+i));
    }
    for(i=0;i<NUM_THREADS;i++){
        pthread_join(threads[i], NULL);
    }
    media=media/N;
    

    printf ("Mean = %ld \n", media);
    
}
