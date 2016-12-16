// new operator should be thread safe

void init (int* m_rndxmix[], int m_nreg, int mixlen)
{
  for ( int ir=1; ir <= m_nreg; ++ir ) {
    m_rndxmix[ ir ] = new int[ mixlen ];                                                                                                 
  } 
}
