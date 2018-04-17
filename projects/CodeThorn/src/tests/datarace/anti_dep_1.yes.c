int main(int argc,char *argv[])
{
  double a[20][20];

#pragma omp parallel for 
  for (int i = 0; i <= 19 - 1; i += 1) {
    for (int j = 0; j <= 20 - 1; j += 1) {
      a[i][j] += a[i + 1][j];
    }
  }
  return 0;
}

