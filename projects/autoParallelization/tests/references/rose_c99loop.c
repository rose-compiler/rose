/*
Contributed by Jeff Keasler
Liao, 10/22/2009
*/
#include "omp.h" 

int main(int argc,char *argv[])
{
  int j;
  int i;
  double a[20UL][20UL];
  for (i = 0; i <= 18; i += 1) {
    
#pragma omp parallel for private (j)
    for (j = 0; j <= 19; j += 1) {
      a[i][j] += a[i + 1][j];
    }
  }
  return 0;
}
// with shadow i and j

void foo(int i,int j)
{
  int j_nom_2;
  int i_nom_1;
  double a[20][20];
  for (i_nom_1 = 0; i_nom_1 <= 18; i_nom_1 += 1) {
    
#pragma omp parallel for private (j_nom_2)
    for (j_nom_2 = 0; j_nom_2 <= 19; j_nom_2 += 1) {
      a[i_nom_1][j_nom_2] += a[i_nom_1 + 1][j_nom_2];
    }
  }
}
