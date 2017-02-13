#include <cstdio>
#include <cassert>
#include <sys/time.h>

struct timeval tGS, tGF, tKS, tKF, tCS, tCF;

// Matrices are stored in row-major order: 
// M(row, col) = M.elements[row * M.width + col] 
typedef struct { 
    int width; 
    int height; 
    float* elements; 
} Matrix; 


// Matrix sum kernel, called by MatrixSum() 
__global__ void MatSumKernel(Matrix A, Matrix B, Matrix C) 
{ 
  int idx = threadIdx.x + blockDim.x * blockIdx.x;
  int idy = threadIdx.y + blockDim.y * blockIdx.y;

  C.elements[idx+C.width*idy] =      
       A.elements[idx+A.width*idy] + B.elements[idx+B.width*idy];
} 

 
// Host code
void MatSum(const Matrix A, const Matrix B, Matrix C) 
{ 

    // Allocate A, B and C in device memory
    int size = A.width*A.height * sizeof(float);
    Matrix d_A; Matrix d_B; Matrix d_C;
    d_A.width = A.width; d_A.height = A.height;
    d_B.width = B.width; d_B.height = B.height;
    d_C.width = C.width; d_C.height = C.height;
    cudaMalloc((void**)&(d_A.elements), size);
    cudaMalloc((void**)&(d_B.elements), size);
    cudaMalloc((void**)&(d_C.elements), size);

    // Load A and B to device memory  
    cudaMemcpy(d_A.elements, A.elements, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B.elements, B.elements, size, cudaMemcpyHostToDevice);      
    cudaMemcpy(d_C.elements, C.elements, size, cudaMemcpyHostToDevice);            

    // Invoke kernel 
    gettimeofday(&tKS, 0);
    dim3 nThreads(16, 16, 1);
    dim3 nBlocks(d_A.width/16, d_B.height/16, 1);
    MatSumKernel<<<nBlocks, nThreads>>>(d_A, d_B, d_C);
    cudaDeviceSynchronize();
    gettimeofday(&tKF, 0);
 
    // Read C from device memory 
    cudaMemcpy(C.elements, d_C.elements, size, cudaMemcpyDeviceToHost);            
 
    // Free device memory 
    cudaFree(d_A.elements);
    cudaFree(d_B.elements);
    cudaFree(d_C.elements);
} 
 
int main(int argc, char **argv) {

  if (argc < 3) {
    printf ("Usage: %s nLines nColumns\n", argv[0]);
    return -1;
  }

  int nLines   = atoi (argv[1]);
  int nColumns = atoi (argv[2]);  
  int i,j;

  Matrix A, B, C;
  A.width = nColumns; A.height = nLines;
  B.width = nColumns; B.height = nLines;
  C.width = nColumns; C.height = nLines;
 
  A.elements = (float *)malloc(A.width*A.height * sizeof(float)); 
  B.elements = (float *)malloc(B.width*B.height * sizeof(float)); 
  C.elements = (float *)malloc(C.width*C.height * sizeof(float)); 
  
  for (i=0; i<A.width*A.height; i++)
    A.elements[i] =   i   % 10 + 1;
  for (i=0; i<B.width*B.height; i++)
    B.elements[i] = (i+1) % 11 + 1;
  for (i=0; i<C.width*C.height; i++)
    C.elements[i] = 0;



  // Performs the sum
  gettimeofday(&tGS, 0);
  MatSum(A, B, C);
  gettimeofday(&tGF, 0);

  //Checks the result
  gettimeofday(&tCS, 0);
  for (i=0; i<nColumns; i++) {
    for (j=0; j<nLines; j++) {
      float ctmp = A.elements[j*nColumns+i] + B.elements[j*nColumns+i];
      assert( fabs(ctmp - C.elements[j*nColumns+i]) < 0.001);
    }
  }
  gettimeofday(&tCF, 0);

  printf(" total=%8.2f\n", (tCF.tv_sec*1000. + tCF.tv_usec/1000.) - (tGS.tv_sec*1000. + tGS.tv_usec/1000.));
  printf("kernel=%8.2f\n", (tKF.tv_sec*1000. + tKF.tv_usec/1000.) - (tKS.tv_sec*1000. + tKS.tv_usec/1000.));
  printf("   gpu=%8.2f\n", (tGF.tv_sec*1000. + tGF.tv_usec/1000.) - (tGS.tv_sec*1000. + tGS.tv_usec/1000.));
  printf("   cpu=%8.2f\n", (tCF.tv_sec*1000. + tCF.tv_usec/1000.) - (tCS.tv_sec*1000. + tCS.tv_usec/1000.));

  return 0;
}

