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
    int c1;
    int c2;
    int c0;
    for (c0 = 0; c0 <= 1; c0++) {
      for (c2 = 2 * c0 + 1; c2 <= 2 * c0 + 14; c2++) {
        B[1][-2 * c0 + c2] = 0.2 * (A[1][-2 * c0 + c2] + A[1][-2 * c0 + c2 - 1] + A[1][1 + (-2 * c0 + c2)] + A[1 + 1][-2 * c0 + c2] + A[1 - 1][-2 * c0 + c2]);
      }
      for (c1 = 2 * c0 + 2; c1 <= 2 * c0 + 14; c1++) {
        B[-2 * c0 + c1][1] = 0.2 * (A[-2 * c0 + c1][1] + A[-2 * c0 + c1][1 - 1] + A[-2 * c0 + c1][1 + 1] + A[1 + (-2 * c0 + c1)][1] + A[-2 * c0 + c1 - 1][1]);
        for (c2 = 2 * c0 + 2; c2 <= 2 * c0 + 14; c2++) {
          A[-2 * c0 + c1 + -1][-2 * c0 + c2 + -1] = B[-2 * c0 + c1 + -1][-2 * c0 + c2 + -1];
          B[-2 * c0 + c1][-2 * c0 + c2] = 0.2 * (A[-2 * c0 + c1][-2 * c0 + c2] + A[-2 * c0 + c1][-2 * c0 + c2 - 1] + A[-2 * c0 + c1][1 + (-2 * c0 + c2)] + A[1 + (-2 * c0 + c1)][-2 * c0 + c2] + A[-2 * c0 + c1 - 1][-2 * c0 + c2]);
        }
        A[-2 * c0 + c1 + -1][14] = B[-2 * c0 + c1 + -1][14];
      }
      for (c2 = 2 * c0 + 2; c2 <= 2 * c0 + 15; c2++) {
        A[14][-2 * c0 + c2 + -1] = B[14][-2 * c0 + c2 + -1];
      }
    }
  }
  
#pragma endscop
  return 0;
}
