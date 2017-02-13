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
 

// Matrix multiplication kernel called by MatrixMul() 
__global__ void MatMulKernel(Matrix A, Matrix B, Matrix C) 
{
int j;
int idx=blockDim.x*blockIdx.x+threadIdx.x;
int idy=blockDim.y*blockIdx.y+threadIdx.y;

for(j=0;j<A.width;j++){
C.elements[idx*C.width+idy]+=A.elements[idx*A.width+j]*B.elements[j*B.width+idy]
}

     
} 
  
// Matrix multiplication - Host code 
void MatMul(const Matrix A, const Matrix B, Matrix C) 
{ 

    // Load A and B to device memory
Matrix *d_a, *d_b, *d_c;

cudaMalloc(&(d_A.elements), A.width*A.height*sizeof(float));
cudaMalloc(&(d_B.elements), B.width*B.height*sizeof(float));
cudaMalloc(&(d_C.elements), C.width*C.height*sizeof(float));

int sizeA= A.width*A.height*sizeof(float);
int sizeB= B.width*B.height*sizeof(float);
int sizeC= C.width*C.height*sizeof(float);

cudaMemcpy(d_A.elements, A.elements, sizeA, cudaMemcpyHostToDevice);
cudaMemcpy(d_B.elements, B.elements, sizeB, cudaMemcpyHostToDevice);
cudaMemcpy(d_C.elements, C.elements, sizeC, cudaMemcpyHostToDevice);
//Define num threads and block size

dim3 nThreads(1,16,1);
dim3 nBlocks(A.height/nThreads.x,C.width/nThreads.y,1);




    // Invoke kernel 
    gettimeofday(&tKS, 0);
    MatMulKernel<<<nBlocks, nThreadsPerBlock>>>(d_A, d_B, d_C);
    gettimeofday(&tKF, 0);





 
    // Read C from device memory 

cudaMemcpy(C.elements, d_C.elements, sizeC, cudaMemcpyDeviceToHost);


 
    // Free device memory 
cudaFree((d_A.elements));
cudaFree((d_B.elements));
cudaFree((d_C.elements));





} 


int main(int argc, char **argv) {

 if (argc < 3) {
    printf ("Usage: %s nLines nColumns\n", argv[0]);
    return -1;
  }

  int nLines   = atoi (argv[1]);
  int nColumns = atoi (argv[2]);  
  int i,j,k;

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

  struct timeval t1;
  gettimeofday(&t1, 0);

  // Perform the multiplication
  gettimeofday(&tGS, 0);
  MatMul(A, B, C);
  gettimeofday(&tGF, 0);

  //Checks the result
  gettimeofday(&tCS, 0);
  for (i=0; i<nLines; i++) {
    for (j=0; j<nLines; j++) {
      float ctmp = 0;
      for (k=0; k<nLines; k++)
	ctmp += A.elements[i*nLines+k] * B.elements[k*nLines+j];
      assert( fabs(ctmp - C.elements[i*nLines+j]) < 0.001);
    }
  }
  gettimeofday(&tCF, 0);

  printf(" total=%8.2f\n", (tCF.tv_sec*1000. + tCF.tv_usec/1000.) - (tGS.tv_sec*1000. + tGS.tv_usec/1000.));
  printf("kernel=%8.2f\n", (tKF.tv_sec*1000. + tKF.tv_usec/1000.) - (tKS.tv_sec*1000. + tKS.tv_usec/1000.));
  printf("   gpu=%8.2f\n", (tGF.tv_sec*1000. + tGF.tv_usec/1000.) - (tGS.tv_sec*1000. + tGS.tv_usec/1000.));
  printf("   cpu=%8.2f\n", (tCF.tv_sec*1000. + tCF.tv_usec/1000.) - (tCS.tv_sec*1000. + tCS.tv_usec/1000.));


  return 0;
}

