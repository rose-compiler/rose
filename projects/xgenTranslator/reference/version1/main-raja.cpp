#include "raja.h"
int main()
{
  const int n =15;
  double* a = new double [n];
  //  double* b = new double [n];

  //  RAJA::forall<RAJA::seq_exec> // the original policy is replaced with new switcher policy
  RAJA::forall<RAJA::switcher_exec>                                                                                         
    (0, n,                                                                                                                  
     [=](int i) {a[i] = 0.5;}                                                                                               
     );                                                                                                                     
                                                                                                                            
                                                                                                                            
  return 0;                                                                                                                 
}

