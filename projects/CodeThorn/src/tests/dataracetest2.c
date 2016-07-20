// NO data race

int main() {
  double a[10][10];
  for(int d=0;d<1;d++) {
#pragma omp parallel for
  for(int i=0;i<10;i++) {
    for(int j=0;j<10;j++) {
      a[i][j]=0;
      #pragma ivdep
      #pragma vector always
      #pragma simd
      for(int k=0;k<10;k++) {
        a[i][k]=1;
      }
    }
  }
  }
  return 0;
}
