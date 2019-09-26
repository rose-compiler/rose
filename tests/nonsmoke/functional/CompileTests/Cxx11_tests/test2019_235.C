// Simplest RAJA example code
// to explore the feasible of building a ROSE-based RAJA lowering tool. 
//
// Even simpler one: only demonstrate the step for template instantiation.
// Liao, 3/9/2017
//
// Compile this code
//    g++ -std=c++11  // GCC 4.9.x is tested. 
//    identityTranslator -rose:Cxx11_only
//#include <cassert>
//#include "raja.h"
// Minimal RAJA header
//#include <stdlib.h>
// Simplest input and output code for as example
namespace RAJA
{
  typedef int Index_type;
  // new specialized policy

 // input code , template 1
  template < typename EXEC_POLICY_T,
           typename LOOP_BODY >
             void forall ( Index_type begin, Index_type end,
                 LOOP_BODY loop_body )
             {
               forall ( EXEC_POLICY_T ( ),
                   begin, end,
                   loop_body );
             }
  // some prebuilt policies
  // the type for specialization

  struct seq_exec 
  {
  }
  ;
  // Some prebuilt specialization for sequential and parallel executions
  template < typename LOOP_BODY >
    void forall ( seq_exec,
        Index_type begin, Index_type end,
        LOOP_BODY loop_body )
    {
      ;
#pragma novector
      for ( Index_type ii = begin; ii < end; ++ ii ) {
        loop_body ( ii );
      }
      ;
    }

  // end namespace
}

//----------------------------------end of min raja.h ------------------------------
// Input RAJA code 
// --------------------------------

struct daxpy_functional_cuda {
  double* a;
  void operator()(int i) {
  a[i] = i+0.5;
  }
};

void foo_input()
{
  const int n = 15;
  double *a = new double [15];

  RAJA::forall< class RAJA::seq_exec  > (0,n, [=] (int i)
      {
      a[i] = 0.5;
      });

  RAJA::forall< class RAJA::seq_exec  > (0,n,  daxpy_functional_cuda{a})
      ;


//  free(a);
}

#if 0
int main()
{
  foo_input();
  return 0;
}
#endif
