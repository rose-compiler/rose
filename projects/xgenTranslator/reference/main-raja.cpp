#include "raja.h"
// insert a prototype here
extern void outline_loop_1(int i, int j, double* a);

int main()
{
  const int n =15;
  double* a = new double [n];
  //  double* b = new double [n];

#if 0 // this code is extracted to be wrapped inside a function
  //  RAJA::forall<RAJA::seq_exec> // the original policy is replaced with new switcher policy
  RAJA::forall<RAJA::switcher_exec>                                                                                         
    (0, n,                                                                                                                  
     [=](int i) {a[i] = 0.5;}                                                                                               
     );                                                                                                                     
#else
  // calling the   
  outline_loop_1 (0,n,a); 
#endif                                                                                                                            
                                                                                                                            
  return 0;                                                                                                                 
}

