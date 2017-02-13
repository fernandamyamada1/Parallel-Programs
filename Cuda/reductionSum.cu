#include <cstdio>
#include <cassert>
#include <sys/time.h>


typedef float vtype;

typedef struct { 
    int size; 
    vtype* elements; 
} Vector; 
 
#define BLOCK_SIZE 512
#define VECTOR_SIZE 1024*1024*10
#define N_BLOCKS VECTOR_SIZE/BLOCK_SIZE
 

__global__ void SumReductionKernel(Vector v, vtype *results) 
{ 

int j;
int idy=blockDim.y*blockIdx.y+threadIdx.y;
int range=VECTOR_SIZE/(N_BLOCKS*BLOCK_SIZE);

__shared__ float vS[2*BLOCK_SIZE];
vS[threadIdx.x]=v.elements[threadIdx.x+(blockDim.x*2)*blockIdx.x];
vS[threadIdx.x+blockDim.x]=v.elements[threadIdx.x+blockDim.x+(blockDim.x*2)*(blockIdx.x+1)];
__syncthreads(); //garantir que todas as threads recebam os seus dados, para nao somar posicoes vazias
//Reducao -multiplas iteracoes
int desloc;
for(desloc=blockDim.x;desloc>0;desloc/=2){
if(threadIdx/desloc==0){
vS[threadIdx.x]+=vS[threadIdx.x+desloc];
}
__syncthreads();




}

int main() {

  int i;

  Vector v_h;
  v_h.size = VECTOR_SIZE;
  v_h.elements = (vtype *)malloc( v_h.size * sizeof(vtype)); 
  
  for (i=0; i<v_h.size; i++) {
    v_h.elements[i] = rand()%10;
  }

  struct timeval t1;
  gettimeofday(&t1, 0);

    // Load A and B to device memory 
    Vector v_d; 
    v_d.size = v_h.size;
    cudaMalloc((void**)&v_d.elements, v_d.size*sizeof(vtype)); 
    cudaMemcpy(v_d.elements, v_h.elements, v_d.size*sizeof(vtype), cudaMemcpyHostToDevice); 

    printf("Allocated %.3f MB of memory.\n", v_d.size*sizeof(vtype)/1024./1024.);

    int nBlocks = N_BLOCKS;
    int nThreadsPerBlock = BLOCK_SIZE;
    vtype *results_d, *results_h;
    cudaMalloc((void**)&results_d, nBlocks * sizeof(vtype)); 
    results_h = (vtype *)malloc(nBlocks * sizeof(vtype));

    struct timeval t2;
    gettimeofday(&t2, 0);


    // Invoke kernel 
    SumReductionKernel<<<nBlocks, nThreadsPerBlock>>>(v_d, results_d); 

    cudaDeviceSynchronize();

    struct timeval t3;
    gettimeofday(&t3, 0);
 
    // Read results from device memory 
    cudaMemcpy(results_h, results_d, nBlocks * sizeof(vtype), cudaMemcpyDeviceToHost); 

    // Free device memory 
    cudaFree(results_d); 
    cudaFree(v_d.elements); 

    double result = 0;
    for (int i=0; i<N_BLOCKS; i++) {
      result += results_h[i];
    }

    cudaError_t error = cudaGetLastError();
    if (error != cudaSuccess) {
      printf("Error: %s\n", cudaGetErrorString(error));
      exit(-1);
    } 

    struct timeval t4;
    gettimeofday(&t4, 0);


  // Evaluates in the CPU
  double result_cpu = 0;
  for (int i=0; i<v_h.size; i++)
    result_cpu += v_h.elements[i];
 
 
  struct timeval t5;
  gettimeofday(&t5, 0);

  printf("transfer=%8.2f\n", (t2.tv_sec*1000. + t2.tv_usec/1000.) - (t1.tv_sec*1000. + t1.tv_usec/1000.));
  printf("     gpu=%8.2f\n", (t3.tv_sec*1000. + t3.tv_usec/1000.) - (t2.tv_sec*1000. + t2.tv_usec/1000.));
  printf("transfer=%8.2f\n", (t4.tv_sec*1000. + t4.tv_usec/1000.) - (t3.tv_sec*1000. + t3.tv_usec/1000.));
  printf("     cpu=%8.2f\n", (t5.tv_sec*1000. + t5.tv_usec/1000.) - (t4.tv_sec*1000. + t4.tv_usec/1000.));
  printf("   total=%8.2f\n", (t5.tv_sec*1000. + t5.tv_usec/1000.) - (t1.tv_sec*1000. + t1.tv_usec/1000.));

  //Checks the result
  printf ( "sum_cpu=%.2f sum_gpu=%.2f diff=%.5f\n", result_cpu, result, abs(result - result_cpu)/result_cpu);
  assert( abs(result - result_cpu)/result_cpu < 0.01);

  free (v_h.elements);
 
  return 0;
}
