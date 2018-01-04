// a test for two pointers without aliasing
#include "omp.h" 

void foo(int m_namix)
{
  int N = m_namix;
  double *p1 = new double [N];
  double *p2 = new double [N];
  
#pragma omp parallel for firstprivate (N)
  for (int i = 0; i <= N - 1; i += 1) {
    p1[i] = i * 2.5;
    p2[i] = i * 0.5;
  }
}
