
#define N 50

void printmatrix( double x[][N]);
void initmatrix( double x[][N], double s);

main()
{
  int i,j, k;
  double a[N][N], b[N][N], c[N][N];

  double s;
  s = 235.0;
  initmatrix(a, s);
  s = 321.0;
  initmatrix(b, s);

  printmatrix(a);
  printmatrix(b);
  for (i = 0; i <= N-1; i+=1) {
    for (j = 0; j <= N-1; j+=1) {
       for (k = 0; k <= N-1; k+=1) {
          c[i][j] = c[i][j] + a[i][k] * b[k][j];
       }
    }
  }

  printmatrix(c);
}
