#   define TSTEPS STEPSIZE
#   define N ARRAYSIZE
# define _PB_TSTEPS STEPSIZE 
# define _PB_N ARRAYSIZE 

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int n = 32;
  int tsteps = 2;
/* Variable declaration/allocation. */
  double A[32][32];
  double B[32][32];
  int t;
  int i;
  int j;
  
#pragma scop
{
    int c1;
    int c2;
    int c0;
    for (c0 = 0; c0 <= 1; c0++) {
      for (c2 = 2 * c0 + 1; c2 <= 2 * c0 + 30; c2++) {
        B[1][-2 * c0 + c2] = 0.2 * (A[1][-2 * c0 + c2] + A[1][-2 * c0 + c2 - 1] + A[1][1 + (-2 * c0 + c2)] + A[1 + 1][-2 * c0 + c2] + A[1 - 1][-2 * c0 + c2]);
      }
      for (c1 = 2 * c0 + 2; c1 <= 2 * c0 + 30; c1++) {
        B[-2 * c0 + c1][1] = 0.2 * (A[-2 * c0 + c1][1] + A[-2 * c0 + c1][1 - 1] + A[-2 * c0 + c1][1 + 1] + A[1 + (-2 * c0 + c1)][1] + A[-2 * c0 + c1 - 1][1]);
        for (c2 = 2 * c0 + 2; c2 <= 2 * c0 + 30; c2++) {
          A[-2 * c0 + c1 + -1][-2 * c0 + c2 + -1] = B[-2 * c0 + c1 + -1][-2 * c0 + c2 + -1];
          B[-2 * c0 + c1][-2 * c0 + c2] = 0.2 * (A[-2 * c0 + c1][-2 * c0 + c2] + A[-2 * c0 + c1][-2 * c0 + c2 - 1] + A[-2 * c0 + c1][1 + (-2 * c0 + c2)] + A[1 + (-2 * c0 + c1)][-2 * c0 + c2] + A[-2 * c0 + c1 - 1][-2 * c0 + c2]);
        }
        A[-2 * c0 + c1 + -1][30] = B[-2 * c0 + c1 + -1][30];
      }
      for (c2 = 2 * c0 + 2; c2 <= 2 * c0 + 31; c2++) {
        A[30][-2 * c0 + c2 + -1] = B[30][-2 * c0 + c2 + -1];
      }
    }
  }
  
#pragma endscop
  return 0;
}
