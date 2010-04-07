/*
Contributed by Jeff Keasler

Liao, 2/16/2010
*/
typedef double real8 ;

class XX
{
  public:
    XX(const int * const ptr) : indarr2(ptr) { }
    int start ;
    int end ;
    int *indarr1 ;
    const int * const indarr2 ;
    int *indarr3 ;
    void foo(real8 aa[], const real8 bb[]) const ;

};

void XX::foo(real8 aa[], const real8 bb[]) const
{
  int i ;
  for (i = start; i < end; ++i) 
  {
    int ii = indarr1[i];
    int jj = indarr3[indarr2[ii]];
    aa[i] = bb[jj];
  }
  return ;
} 

