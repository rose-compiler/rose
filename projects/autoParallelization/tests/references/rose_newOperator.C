// new operator should be thread safe
#include "omp.h" 

void init(int *m_rndxmix[],int m_nreg,int mixlen)
{
  
#pragma omp parallel for firstprivate (m_nreg)
  for (int ir = 1; ir <= m_nreg; ir += 1) {
    m_rndxmix[ir] = (new int [mixlen]);
  }
}
