// Simple example of how to cause nicely vectorized code to be generated.

#define SIZE 100

void test4(double * restrict a, double * restrict b)
   {
     int i;

     double *x = __builtin_assume_aligned(a, 16);
     double *y = __builtin_assume_aligned(b, 16);

     for (i = 0; i < SIZE; i++)
        {
          x[i] += y[i];
        }
   }
