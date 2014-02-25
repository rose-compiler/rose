
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
    int c0;
    int c2;
    int c1;
    beta = 1;
    for (c0 = 0; c0 <= 9; c0++) {
      for (c1 = 0; c1 <= 9; c1++) {
        D[c0][c1] *= beta;
        for (c2 = 0; c2 <= 9; c2++) {
          D[c0][c1] += tmp[c0][c2] * C[c2][c1];
        }
      }
    }
  }
}
