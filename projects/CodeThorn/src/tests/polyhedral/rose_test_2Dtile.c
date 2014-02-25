
int main()
{
  int i;
  int j;
  int k;
  int beta;
  int C[10][10];
  int D[10][10];
  int tmp[10][10];
{
    int c5;
    int c1;
    int c2;
    int c0;
    int c3;
    beta = 1;
#pragma omp parallel for private(c3, c2, c1, c5)
    for (c0 = 0; c0 <= 4; c0++) {
      for (c1 = 0; c1 <= 4; c1++) {
        for (c2 = 0; c2 <= 9; c2++) {
          if (c2 == 0) {
            for (c3 = 2 * c0; c3 <= 2 * c0 + 1; c3++) {
#pragma ivdep
#pragma vector always
#pragma simd
              for (c5 = 2 * c1; c5 <= 2 * c1 + 1; c5++) {
                D[c3][c5] *= beta;
                D[c3][c5] += tmp[c3][0] * C[0][c5];
              }
            }
          }
          if (c2 >= 1) {
            for (c3 = 2 * c0; c3 <= 2 * c0 + 1; c3++) {
#pragma ivdep
#pragma vector always
#pragma simd
              for (c5 = 2 * c1; c5 <= 2 * c1 + 1; c5++) {
                D[c3][c5] += tmp[c3][c2] * C[c2][c5];
              }
            }
          }
        }
      }
    }
  }
}
