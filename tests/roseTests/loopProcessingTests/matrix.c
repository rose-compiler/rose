#include <stdio.h>

#define n 50
#define N 50

void initmatrix( double x[][N], double s)
{
         int i,j;
         for (j = 0; j<= N-1; j+=1) {
           for ( i = 0; i <= N-1; i+=1)
              x[j][i] = i*s /111.0 + j * s / 57.0;
         }
}

void printmatrix( double x[][N])
{
         int i,j;
         for (j = 0; j<= N-1; j+=1)
           for ( i = 0; i <= N-1; i+=1)
              printf("%f ", x[i][j]);
}

