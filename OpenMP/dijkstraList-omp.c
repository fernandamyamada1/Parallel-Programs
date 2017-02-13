#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <omp.h>
#include <sys/time.h>

struct Graph {
    int nVertices;
    int *nArestas;
    int **arestas;
    int **w;
};

struct Graph *createRandomGraph(int nVertices, int nArestas, int seed) {
    
    srandom(seed);
    
    struct Graph *graph = (struct Graph *) malloc( sizeof(struct Graph) );
    graph->nVertices = nVertices;
    graph->nArestas = (int *)  malloc( sizeof(int)    * nVertices );
    graph->arestas  = (int **) malloc( sizeof(int *)  * nVertices );    
    graph->w        = (int **) malloc( sizeof(int *)  * nVertices );    

    int k,v;
#
    for (v=0; v<nVertices; v++) {
        graph->arestas[v] = (int *) malloc( sizeof(int) * nVertices );            
        graph->w[v]       = (int *) malloc( sizeof(int) * nVertices );            
        graph->nArestas[v] = 0;
    }

    for (k=0; k<nArestas; k++) {
        int source = random() % nVertices;
        int dest   = random() % nVertices;
        int w      = 1 + (random() % 10);
        while (source == dest)
            dest = random() % nVertices;
        graph->arestas[source][ graph->nArestas[source] ] = dest;
        graph->arestas[dest][ graph->nArestas[dest] ] = source;                    
        
        graph->w[source][ graph->nArestas[source]++ ] = w;
        graph->w[dest][ graph->nArestas[dest]++ ] = w;                    
    }

    return graph;
}

int *dijkstra(struct Graph *graph, int source) {
    
    int nVisitados = 0;
    int nVertices = graph->nVertices;
    int *visitados = (int *) malloc( sizeof(int) * nVertices );
    int *l = (int *) malloc( sizeof(int) * nVertices );
    
    int k,v;
    for (v=0; v<nVertices; v++) {
        l[v] = INT_MAX;
        visitados[v] = 0;
    }    
    l[source] = 0;

    
    for (v=0; v<nVertices; v++) {
        
        int min = 0;
        int minValue = INT_MAX;
        
#pragma omp parallel
        {
            
            int minL = 0;
            int minValueL = INT_MAX;
#pragma omp for
        for (k=0; k<nVertices; k++)
            if ( visitados[k] == 0 && l[k] < minValue ) {
                minValueL = l[k];
                minL = k;
            }
            
#pragma omp critical 
            if(minValueL<minValue){
                minValue=minValueL;
                min=minL;
            }
        
        visitados[min] = 1;
#pragma omp for
        for (k=0; k < graph->nArestas[min]; k++) {
            int dest = graph->arestas[min][k];
            if ( l[ dest ] > l[min] + graph->w[min][k] )
                l[ dest ] = l[min] + graph->w[min][k];                
        }
        
        }
    }
    
    free(visitados);
    
    return l;
}

int main(int argc, char ** argv) {
    
    int i;
    for (i=0; i<1; i++) {
        int nVertices = 20000;
        int nArestas  = nVertices*100;
        int seed = i;
        
        struct Graph *graph = createRandomGraph(nVertices, nArestas, seed);

  printf("Starting Dijkstra.\n");
  struct timeval t1;
  gettimeofday(&t1, 0);

        int *dist = dijkstra(graph,0);
    
  struct timeval t2;
  gettimeofday(&t2, 0);
  printf("time=%8.2f\n", (t2.tv_sec*1000. + t2.tv_usec/1000.) - (t1.tv_sec*1000. + t1.tv_usec/1000.));
    
	double mean = 0;
        int v;
        for (v=0; v<graph->nVertices; v++)  {
	  mean += dist[v];
	}

	printf("Mean distance = %.2f\n", mean / nVertices);

	for (v=0; v<nVertices; v++) {
	  free(graph->arestas[v]);
	  free(graph->w[v]);
	}
	free(graph->nArestas);
	free(graph->arestas);
	free(graph->w);    
	free(graph);
        
        free(dist);
    }
}
