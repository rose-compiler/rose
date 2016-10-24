double abs(double x) { if (x < 0) return -x; else return x; }

#define n 50
void printmatrix( double x[][n]);
void initmatrix( double x[][n], double s);

main(int argc, char* argv[]) {
int p[n], i, j, k;
double a[n][n], mu, t;

initmatrix(a, 5.0);
printmatrix(a);

for (k = 0; k<=n-2; k+=1) {
    p[k] = k;
    mu = abs(a[k][k]);
    for (i = k+1; i <= n-1; i+=1) {
      if (mu < abs(a[i][ k])) {
        mu = abs(a[i][k]);
        p[k] = i;
      }
    }

    for (j = k; j <= n-1; j+=1) {
       t = a[k][j];
       a[k][j] = a[p[k]][j];
       a[p[k]][j] = t;
    }

    for (i = k+1; i <= n-1; i+=1) {
          a[i][k] = a[i][k]/a[k][k];
    }
    for (j = k+1; j <=n-1; j+=1) {
       for (i = k+1; i <=n-1; i+=1) {
            a[i][j] = a[i][j] - a[i][k]*a[k][j];
       }
    }
 }

printmatrix(a);
}
