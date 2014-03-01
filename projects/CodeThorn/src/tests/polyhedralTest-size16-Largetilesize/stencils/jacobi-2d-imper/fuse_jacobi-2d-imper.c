#   define TSTEPS STEPSIZE
#   define N ARRAYSIZE
# define _PB_TSTEPS STEPSIZE 
# define _PB_N ARRAYSIZE 

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int n = 16;
  int tsteps = 2;
/* Variable declaration/allocation. */
  double A[16][16];
  double B[16][16];
  int t;
  int i;
  int j;
  
#pragma scop
{
    int c3;
    int c5;
    int c1;
    for (c1 = 0; c1 <= 1; c1++) {
      for (c3 = 1; c3 <= 14; c3++) {
        for (c5 = 1; c5 <= 14; c5++) {
          B[c3][c5] = 0.2 * (A[c3][c5] + A[c3][c5 - 1] + A[c3][1 + c5] + A[1 + c3][c5] + A[c3 - 1][c5]);
        }
      }
      for (c3 = 1; c3 <= 14; c3++) {
        for (c5 = 1; c5 <= 14; c5++) {
          A[c3][c5] = B[c3][c5];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
