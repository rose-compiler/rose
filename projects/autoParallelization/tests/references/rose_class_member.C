/*
Contributed by Jeff Keasler
Liao, 2/16/2010
*/
#include "omp.h" 
typedef double real8;

class XX 
{
  

  public: inline XX(const int *const ptr) : indarr2(ptr)
{
  }
  int start;
  int end;
  int *indarr1;
  const int *const indarr2;
  int *indarr3;
  void foo(real8 aa[],const real8 bb[]) const;
}
;

void XX::foo(real8 aa[],const real8 bb[]) const
{
  int i;
  
#pragma omp parallel for private (i)
  for (i = (this) -> start; i <= (this) -> end - 1; i += 1) {
    int ii = (this) -> indarr1[i];
    int jj = (this) -> indarr3[(this) -> indarr2[(this) -> indarr1[i]]];
    aa[i] = bb[(this) -> indarr3[(this) -> indarr2[(this) -> indarr1[i]]]];
  }
  return ;
}
