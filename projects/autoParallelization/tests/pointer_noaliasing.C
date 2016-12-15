//! Compilers should figure out freshly allocated pointers do not alias to each other
void PdVWorkBaseLoopAlgorithm_setup(int m_namix)
{
  int N          = m_namix;

  double * m_pdv_sum      = new double[ N ];
  double * m_nvol         = new double[ N ];
  double * m_ovol         = new double[ N ];

//#pragma omp parallel for schedule(static)
  for ( int i=0; i < N; ++i ) { 
    m_pdv_sum[ i ] = 0.0;
    m_nvol[ i ]   = i*2.5;
    m_ovol[ i ]   = i*0.5;
  }


}

