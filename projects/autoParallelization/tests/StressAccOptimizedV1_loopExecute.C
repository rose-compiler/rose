class StructuredDomain
{
public:
  StructuredDomain();
  double* m_sxx;

  int m_frz;
  int m_lpz;

  double* m_sxy;
private:

  StructuredDomain(const StructuredDomain& );
  StructuredDomain& operator=(const StructuredDomain& );
};

class StressAccOptimizedV1 
{
public:

  StressAccOptimizedV1( const StructuredDomain* domain,
                              const unsigned numberOfIterations );
protected:
  const StructuredDomain* m_Domain;
  virtual void loopExecute();

private:
  StressAccOptimizedV1() {};
};

void StressAccOptimizedV1::loopExecute()
{
  int i,j,ii,jj;
  int lb = m_Domain->m_frz;
  int ub = m_Domain->m_lpz + 1;

  const int nb = 2048;
  int REM = (ub-lb) % nb;
  int ii_ub = ub - REM;

  double sxx_t[nb],
         sxy_t[nb];

  for (int ii = lb; ii < ii_ub; ii+=nb) {
    int i_ub = ii + nb;

    // Must have this loop to trigger the bug
    for ( i = ii, jj=0 ; i < i_ub ; i++, jj++ ) {
      sxx_t[jj] = m_Domain->m_sxx[i];
    }

    for ( i = ii, jj = 0 ; i < i_ub ; i++, jj++ ) {
      sxy_t[jj] = m_Domain->m_sxy[i];
    } 

  }
}
