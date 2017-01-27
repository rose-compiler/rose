#include "omp.h" 

class StructuredDomain 
{
  public: StructuredDomain();
  double *m_sxx;
  int m_frz;
  int m_lpz;
  double *m_sxy;
  private: StructuredDomain(const class StructuredDomain &);
  StructuredDomain &operator=(const class StructuredDomain &);
}
;

class StressAccOptimizedV1 
{
  public: StressAccOptimizedV1(const class StructuredDomain *domain,const unsigned int numberOfIterations);
  protected: const class StructuredDomain *m_Domain;
  virtual void loopExecute();
  

  private: inline StressAccOptimizedV1()
{
  }
}
;

void StressAccOptimizedV1::loopExecute()
{
  int i;
  int j;
  int ii;
  int jj;
  int lb = (this) -> m_Domain -> m_frz;
  int ub = (this) -> m_Domain -> m_lpz + 1;
  const int nb = 2048;
  int REM = (ub - lb) % nb;
  int ii_ub = ub - REM;
  double sxx_t[2048];
  double sxy_t[2048];
  
#pragma omp parallel for private (i,jj) firstprivate (ii_ub)
  for (int ii = lb; ii <= ii_ub - 1; ii += nb) {
    int i_ub = ii + nb;
// Must have this loop to trigger the bug
    for ((i = ii , jj = 0); i <= i_ub - 1; (i++ , jj++)) {
      sxx_t[jj] = (this) -> m_Domain -> m_sxx[i];
    }
    for ((i = ii , jj = 0); i <= i_ub - 1; (i++ , jj++)) {
      sxy_t[jj] = (this) -> m_Domain -> m_sxy[i];
    }
  }
}
