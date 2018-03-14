// test  autoPar -rose:autopar:unique_indirect_index
// unparsing error: u[k][j][i][m] becomes u[k][0][i][m]
// This error is triggered by uniformIndirectIndexedArrayRefs()
#define PROBLEM_SIZE   24
#define IMAX      PROBLEM_SIZE
#define JMAX      PROBLEM_SIZE
#define KMAX      PROBLEM_SIZE
#define IMAXP     IMAX/2*2
#define JMAXP     JMAX/2*2
#include "omp.h" 
double u[24][24 / 2 * 2 + 1][24 / 2 * 2 + 1][5];
void exact_solution(double xi,double eta,double zeta,double dtemp[5]);
double dnym1;
double dnzm1;
int grid_points[3];
//---------------------------------------------------------------------
// This subroutine initializes the field variable u using
// tri-linear transfinite interpolation of the boundary values
//---------------------------------------------------------------------

void initialize()
{
  int i;
  int j;
  int k;
  int m;
  int ix;
  int iy;
  int iz;
  double xi;
  double eta;
  double zeta;
  double Pface[2][3][5];
  double Pxi;
  double Peta;
  double Pzeta;
  double temp[5];
//---------------------------------------------------------------------
// west face
//---------------------------------------------------------------------
  i = 0;
  xi = 0.0;
  for (k = 0; k <= grid_points[2] - 1; k += 1) {
    zeta = ((double )k) * dnzm1;
    for (j = 0; j <= grid_points[1] - 1; j += 1) {
      eta = ((double )j) * dnym1;
      exact_solution(xi,eta,zeta,temp);
      
#pragma omp parallel for private (m) firstprivate (i)
      for (m = 0; m <= 4; m += 1) {
        u[k][j][i][m] = temp[m];
      }
    }
  }
}
