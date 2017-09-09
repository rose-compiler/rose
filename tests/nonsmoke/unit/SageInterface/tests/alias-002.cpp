// a test for two pointers without aliasing
void foo (int m_namix)
{
  int N = m_namix;
  double * p1= new double[N];
  double * p2= new double[N];

  for ( int i=0; i < N; ++i ) { 
    p1 [i] = i*2.5;
    p2 [i]= i*0.5;
  }
}

