// functions within a namespace
// 
namespace abc {
  class StressWorkBaseLoopAlgorithm
  {
    double * m_dele, * m_volold, * m_volnew; 
    int m_namix;
    void setup();
  };


  void StressWorkBaseLoopAlgorithm::setup()
  {
    //  const int N = m_Domain->m_namix;
    const int N = m_namix;

    m_dele = new double[ N ];
#if 0
    m_dt = 0.5;
    m_exx  = new double[ N ];
    m_exy  = new double[ N ];
    m_exz  = new double[ N ];
    m_eyy  = new double[ N ];
    m_eyz  = new double[ N ];
    m_ezz  = new double[ N ];
#endif
    m_volold = new double[ N ];
    m_volnew = new double[ N ];

    //#pragma omp parallel for schedule(static)
    for ( int i=0; i < N; ++i ) {
      m_dele[ i ]   = 0.0;
      m_volold[ i ] = 2.0;
      m_volnew[ i ] = 3.5;
    }

#if 0
    memcpy(m_exx,m_Domain->m_sxx,N*sizeof(double));
    memcpy(m_exy,m_Domain->m_sxy,N*sizeof(double));
    memcpy(m_exz,m_Domain->m_sxz,N*sizeof(double));
    memcpy(m_eyy,m_Domain->m_syy,N*sizeof(double));
    memcpy(m_eyz,m_Domain->m_syz,N*sizeof(double));
    memcpy(m_ezz,m_Domain->m_szz,N*sizeof(double));
#endif
  }

}

