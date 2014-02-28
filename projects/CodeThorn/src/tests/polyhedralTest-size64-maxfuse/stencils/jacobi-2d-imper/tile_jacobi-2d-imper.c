#   define TSTEPS STEPSIZE
#   define N ARRAYSIZE
# define _PB_TSTEPS STEPSIZE 
# define _PB_N ARRAYSIZE 

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int n = 64;
  int tsteps = 2;
/* Variable declaration/allocation. */
  double A[64][64];
  double B[64][64];
  int t;
  int i;
  int j;
  
#pragma scop
{
    int c1;
    int c2;
    int c0;
{
      int c10;
      int c11;
      int c3;
      for (c3 = 0; c3 <= 1; c3++) {
        for (c10 = 2 * c3 + 2; c10 <= 2 * c3 + 62; c10++) {
          for (c11 = 2 * c3 + 2; c11 <= 2 * c3 + 62; c11++) {
            B[- 2 * c3 + c10][- 2 * c3 + c11] = 0.2 * (A[- 2 * c3 + c10][- 2 * c3 + c11] + A[- 2 * c3 + c10][- 2 * c3 + c11 - 1] + A[- 2 * c3 + c10][1 + (- 2 * c3 + c11)] + A[1 + (- 2 * c3 + c10)][- 2 * c3 + c11] + A[- 2 * c3 + c10 - 1][- 2 * c3 + c11]);
          }
        }
        for (c10 = 2 * c3 + 2; c10 <= 2 * c3 + 62; c10++) {
          B[- 2 * c3 + c10][1] = 0.2 * (A[- 2 * c3 + c10][1] + A[- 2 * c3 + c10][1 - 1] + A[- 2 * c3 + c10][1 + 1] + A[1 + (- 2 * c3 + c10)][1] + A[- 2 * c3 + c10 - 1][1]);
        }
        for (c10 = 2 * c3 + 2; c10 <= 2 * c3 + 63; c10++) {
          A[62][- 2 * c3 + c10 + - 1] = B[62][- 2 * c3 + c10 + - 1];
        }
        for (c10 = 2 * c3 + 1; c10 <= 2 * c3 + 62; c10++) {
          B[1][- 2 * c3 + c10] = 0.2 * (A[1][- 2 * c3 + c10] + A[1][- 2 * c3 + c10 - 1] + A[1][1 + (- 2 * c3 + c10)] + A[1 + 1][- 2 * c3 + c10] + A[1 - 1][- 2 * c3 + c10]);
        }
        for (c10 = 2 * c3 + 2; c10 <= 2 * c3 + 62; c10++) {
          A[- 2 * c3 + c10 + - 1][62] = B[- 2 * c3 + c10 + - 1][62];
        }
        for (c10 = 2 * c3 + 2; c10 <= 2 * c3 + 62; c10++) {
          for (c11 = 2 * c3 + 2; c11 <= 2 * c3 + 62; c11++) {
            A[- 2 * c3 + c10 + - 1][- 2 * c3 + c11 + - 1] = B[- 2 * c3 + c10 + - 1][- 2 * c3 + c11 + - 1];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
