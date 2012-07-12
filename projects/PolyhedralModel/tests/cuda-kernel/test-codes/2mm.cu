/*********************************************************************************/
//
// Polybench kernels implementation on CUDA GPU
//
// Computer & Information Science, University of Delaware
// Author(s):   Sudhee Ayalasomayajula (sudhee1@gmail.com)
//              John Cavazos (cavazos@cis.udel.edu)
//		Scott Grauer Gray(sgrauerg@gmail.com)
//              Robert Searles (rsearles35@aol.com)   
//              Lifan Xu (xulifan@udel.edu)
//
// Contact(s): Lifan Xu (xulifan@udel.edu)
// Reference(s):
//
/*********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>

/* Default problem size. */
# define NI 512
# define NJ 512
# define NK 512
# define NL 512

typedef double real;
#define GPU_DEVICE 0

real C[NI][NJ];
real A[NI][NK];
real B[NK][NJ];
real D[NJ][NL];
real E[NI][NL];
real F[NI][NL];

real *A_gpu;
real *B_gpu;
real *C_gpu;
real *D_gpu;
real *E_gpu;

double t_start, t_end;

void init_array()
{
  int i, j;


  for (i = 0; i < NI; i++)
    for (j = 0; j < NK; j++)
      A[i][j] = 1;
  for (i = 0; i < NK; i++)
    for (j = 0; j < NJ; j++)
      B[i][j] = 2;
  for (i = 0; i < NI; i++)
    for (j = 0; j < NJ; j++)
      C[i][j] = 3;
  for (i = 0; i < NJ; i++)
    for (j = 0; j < NL; j++)
      D[i][j] = 4;
  for (i = 0; i < NI; i++)
    for (j = 0; j < NL; j++)
      E[i][j] = 5;
  for (i = 0; i < NI; i++)
    for (j = 0; j < NL; j++)
      F[i][j] = 5;
	
}

double rtclock()
{
    struct timezone Tzp;
    struct timeval Tp;
    int stat;
    stat = gettimeofday (&Tp, &Tzp);
    if (stat != 0) printf("Error return from gettimeofday: %d",stat);
    return(Tp.tv_sec + Tp.tv_usec*1.0e-6);
}



void compareResults(){
	int i,j,fail;
	fail = 0;

	for (i=0; i<NI; i++)
	{
		for (j=0; j<NL; j++)
		{
			if (abs(E[i][j]) < abs(F[i][j])){
				if ((abs(F[i][j]) - abs(E[i][j])) > 0.1){
					fail++;
				}
			}
			else {
				if ((abs(E[i][j]) - abs(F[i][j])) > 0.1){
					fail++;
				}
			}
		}
	}
	
	// print results
	printf("Number of misses: %d\n", fail);
}

void GPU_argv_init()
{
	cudaDeviceProp deviceProp;
	cudaGetDeviceProperties(&deviceProp, GPU_DEVICE);
	printf("setting device %d with name %s\n",GPU_DEVICE,deviceProp.name);
	cudaSetDevice( GPU_DEVICE );
	
	return;
}

void GPU_mem_init()
{
	cudaMalloc((void **)&A_gpu, sizeof(real) * NI * NK);
	cudaMalloc((void **)&B_gpu, sizeof(real) * NK * NJ);
	cudaMalloc((void **)&C_gpu, sizeof(real) * NI * NJ);
	cudaMalloc((void **)&D_gpu, sizeof(real) * NJ * NL);
	cudaMalloc((void **)&E_gpu, sizeof(real) * NI * NL);
	cudaMemcpy(A_gpu, A, sizeof(real) * NI * NK, cudaMemcpyHostToDevice);
	cudaMemcpy(B_gpu, B, sizeof(real) * NK * NJ, cudaMemcpyHostToDevice);
	cudaMemcpy(C_gpu, C, sizeof(real) * NK * NJ, cudaMemcpyHostToDevice);
	cudaMemcpy(D_gpu, D, sizeof(real) * NJ * NL, cudaMemcpyHostToDevice);
	cudaMemcpy(E_gpu, E, sizeof(real) * NI * NL, cudaMemcpyHostToDevice);	
	
}


__global__ void mm2_kernel(real *a, real *b, real *c, int dim0, int dim1, int dim2)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	int j = blockIdx.y * blockDim.y + threadIdx.y;
	int k;
	int tid = i * dim0 + j;
	for(k=0; k< dim0; k++)
	{
		c[tid] += a[i * dim0 + k] * b[k * dim0 +j];
	}
	
	return;
}

void mm2_cpu(real A[NI][NK], real B[NK][NJ], real C[NI][NJ], real D[NJ][NL], real E[NI][NL], int ni, int nj, int nk, int nl)
{
	int i,j,k;
	
  	for (i = 0; i < ni; i++){
    		for (j = 0; j < nj; j++)
	      {
	
		for (k = 0; k < nk; ++k)
		  C[i][j] += A[i][k] * B[k][j];
	      }
	}
	for (i = 0; i < ni; i++){
		for (j = 0; j < nl; j++)
	      {
	
		for (k = 0; k < nj; ++k)
		  E[i][j] += C[i][k] * D[k][j];
	      }
	}
}

int main(int argc, char** argv)
{

	int ni = NI;
	int nj = NJ;
	int nk = NK;
	int nl = NL;

  	init_array();
	 
	GPU_argv_init();

	GPU_mem_init();
		
	dim3 block(32,8);
	dim3 grid1(NI/32,NJ/8);
	dim3 grid2(NI/32,NL/8);
	t_start = rtclock();
	mm2_kernel<<<grid1,block>>>(A_gpu,B_gpu,C_gpu,ni,nk,nj);
	mm2_kernel<<<grid2,block>>>(C_gpu,D_gpu,E_gpu,ni,nj,nl);
	cudaThreadSynchronize();
	t_end = rtclock();
	cudaMemcpy(E, E_gpu, sizeof(real) * NI * NL, cudaMemcpyDeviceToHost);

	fprintf(stdout, "GPU Runtime: %0.6lfs\n", t_end - t_start);

	t_start = rtclock();

	mm2_cpu(A, B, C, D, F, ni, nj, nk, nl);

	t_end = rtclock();

	fprintf(stdout, "CPU Runtime: %0.6lfs\n", t_end - t_start);

	compareResults();

  return 0;
}
