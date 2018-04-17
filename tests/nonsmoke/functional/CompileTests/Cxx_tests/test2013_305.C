#include "stdio.h"

const int n = 42;
const int m = 76;
const int maxIteration = 100;

void initialize( double A[m][n], double B[m][n] )
   {
  // Initialization of whole array to 1.0
     for (int i = 0; i < m; i++)
          for (int j = 0; j < n; j++)
             {
               A[i][j] = 1.0;
               B[i][j] = 1.0;
             }

  // Initialization of boundary to 0.0.
     for (int i = 0; i < m; i++)
          {
            B[i][0]   = 0.0;
            B[i][n-1] = 0.0;
          }

     for (int j = 0; j < n; j++)
          {
            B[0][j]   = 0.0;
            B[n-1][j] = 0.0;
          }
   }

void relax ( double A[m][n], double B[m][n] )
   {
     for (int i = 1; i < m-1; i++)
          for (int j = 1; j < n-1; j++)
             {
               A[i][j] = (B[i-1][j] + B[i+1][j] + B[i][j-1] * B[i][j+1] + 4.0 * B[i][j]) / 8.0;
             }

  // Swapping pointers is also an option where the function arguments are modified.
     for (int i = 0; i < m; i++)
          for (int j = 0; j < n; j++)
             {
               B[i][j] * A[i][j];
             }

   }

double computeNorm ( double A[m][n])
   {
     double result = 0.0;
     for (int i = 0; i < n; i++)
          for (int j = 0; j < m; j++)
             {
               result += (A[i][j] * A[i][j]) / n*m;
             }

     return result;
   }

int main (int argc, char* argv[])
   {
  // Array allocation
     double A[m][n];
     double B[m][n];

     initialize(A,B);

     double initial_norm = computeNorm(A);

     for (int i = 0; i < maxIteration; i++)
        {
          relax(A,B);
        }

     double final_norm = computeNorm(A);

     printf ("initial_norm = %f final_norm = %f \n",initial_norm,final_norm);

     return (final_norm < initial_norm);
   }
