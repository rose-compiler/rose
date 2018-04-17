// a test for two pointers sharing the same base address.
void foo (int m_namix, double* third)
{
  int N = m_namix;
  double * base = new double[ N+1 ];
  double * p1=  base; 
  double * p2= p1 + 1; 

  for ( int i=0; i < N; ++i ) {
    p1 [ i ]   = i*2.5;
    p2 [ i ]   = i*0.5;
    third [ i ] = i*0.5;
  }
}

