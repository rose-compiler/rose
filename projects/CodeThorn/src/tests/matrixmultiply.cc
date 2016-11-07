
void mxm44_0(float* A[],int N1, float* B[],int N2, float* C[], int N3)
{
  int k;
  int i;
  int j;
  for (j = 0; j <  N3; j = j + 1) {
    for (i = 0; i <  N1; i = i + 1) {
      C[j][i] = 0.0f;
      for (k = 0; k <  N2; k = k + 1) {
        C[j][i] = C[j][i] + A[k][i] * B[j][k];
      }
    }
  }
  return ;
}
