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
{
      int c2;
      int c6;
      int c8;
      for (c2 = 0; c2 <= 1; c2++) {
        for (c6 = 1; c6 <= 14; c6++) {
          for (c8 = 1; c8 <= 14; c8++) {
            B[c6][c8] = 0.2 * (A[c6][c8] + A[c6][c8 - 1] + A[c6][1 + c8] + A[1 + c6][c8] + A[c6 - 1][c8]);
          }
        }
        for (c6 = 1; c6 <= 14; c6++) {
          for (c8 = 1; c8 <= 14; c8++) {
            A[c6][c8] = B[c6][c8];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
