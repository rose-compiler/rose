#include <cstdlib>

struct SVector {
  double rho;
  double u[2];
};

int main() {
  SVector** V;
  int n=5;
  V = (SVector**) calloc((2 * n + 2), sizeof(SVector*));
  for(int i=0;i<n;i++) {
    V[0][i].rho=-V[1][i].rho;
    V[0][i].u[0]=-V[1][i].u[0];
  }
  return 0;
}
