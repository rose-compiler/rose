/* A kernel for two level parallelizable loop with reduction */
#include "omp.h" 
float u[100][100];

float foo()
{
  int i;
  int j;
  float temp;
  float error;
  
#pragma omp parallel for private (temp,i,j) reduction (+:error)
  for (i = 0; i <= 99; i += 1) {
    
#pragma omp parallel for private (temp,j) reduction (+:error)
    for (j = 0; j <= 99; j += 1) {
      temp = u[i][j];
      error = error + temp * temp;
    }
  }
  return error;
}
