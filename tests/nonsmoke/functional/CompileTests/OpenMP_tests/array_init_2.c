// Test the handling of two loops under omp for
// watch the loop index replacement (private by default)
// and tje array outlining
#include <stdlib.h>

int main(void)
{
  int i, j;
  float** u = (float**) malloc( 500 * sizeof( float*) );
  for( i=0; i<500; i++)
    u[i] = (float*) malloc( 500 * sizeof(float) );

#pragma omp parallel for
  for (i=0; i<500; i++)
    for (j=0; j<500; j++)
    {
      u[i][j] = 0.0;
    }

  return 0;
}
// This code has data races since j is shared
