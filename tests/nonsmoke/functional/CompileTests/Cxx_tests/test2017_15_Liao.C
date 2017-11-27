// a test for two pointers sharing the same base address.
void foo (int m_namix)
{
  int N = m_namix;
  double * base = new double[ N+1 ];
  double * m_nvol =  base; 
  double * m_ovol = m_nvol +1; 

  for ( int i=0; i < N; ++i ) {
    m_nvol[ i ]   = i*2.5;
    m_ovol[ i ]   = i*0.5;
  }
}

void foo1 (int sz)
{
  double * base = new double [sz+1];

  double *x1 =base; 
  double *x2 = x1+1;
  for ( int i=0; i < sz; ++i ) {
    if (x1[i]<0.0)
      x1[ i ] = 1.0;

    if (x2[i]>0.0)
      x2[ i ] -= -1.0;

    x2[i]+=x1[i]; 
  }
}
