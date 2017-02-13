#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/time.h>

struct Graph {
    int nVertices;
    int **w;
};

struct Graph *createRandomGraph(int nVertices, int nArestas, int seed) {

    int k,v;    
    srandom(seed);
    
    struct Graph *graph = (struct Graph *) malloc( sizeof(struct Graph) );
    graph->nVertices = nVertices;
    graph->w = (int **) malloc( sizeof(int *)  * nVertices );    
    for (v=0; v<nVertices; v++) {
        graph->w[v] = (int *) malloc( sizeof(int) * nVertices );            
        for (k=0; k<nVertices; k++)
            graph->w[v][k] = 0; // Division prevents overflows
    }

    for (k=0; k<nArestas; k++) {
        int source = random() % nVertices;
        int dest   = random() % nVertices;
        while (source == dest)
            dest = random() % nVertices;

        int w      = 1 + (random() % 10);        
        graph->w[source][ dest ] = w;
        graph->w[dest][ source ] = w;                    
    }
    
    return graph;
}

int *dijkstra(struct Graph *graph, int source) {

    int k,v;    
    int nVisitados = 0;
    int nVertices = graph->nVertices;

    int *visitados = (int *) malloc( sizeof(int) * nVertices );
    int *l = (int *) malloc( sizeof(int) * nVertices );
    
    for (k=0; k<nVertices; k++) {
        l[k] = graph->w[source][k];
        visitados[k] = 0;
    }
    l[source] = 0;
    visitados[source]=1;
    
    for (v=1; v<nVertices; v++) {
        
        int min = 0;
        int minValue = INT_MAX;
#pragma omp parallel
        {
            int minL=0;
            int minValueL=0;
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
        }

        visitados[min] = 1;
#pragma omp parallel for
        for (k=0; k < nVertices; k++)
            if ( l[k] > l[min] + graph->w[min][k] )
                l[k] = l[min] + graph->w[min][k];                
        
    }
#pragma omp critical
    {

    printf("Graph={");
    for (v=0; v<nVertices; v++) {
        printf("{");
        for (k=0; k<nVertices; k++){
            if(k!=nVertices-1)
                printf("%d, ", graph->w[v][k]);
            else
                printf("%d", graph->w[v][k]);
        }
        printf("}, \n");
    }
    printf("}; \n");

    }


    free(visitados);

    return l;
}

int main(int argc, char ** argv) {
    
    int i, v, k;
    for (i=0; i<1; i++) {
        int nVertices = 150;
        int nArestas  = nVertices*10;
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
        for (v=0; v<graph->nVertices; v++)
	  mean += dist[v];

	printf("Mean distance = %.2f\n", mean / nVertices);

        
	for (v=0; v<nVertices; v++)
	  free(graph->w[v]);
	free(graph->w);    
	free(graph);
        
        free(dist);
    }
}
