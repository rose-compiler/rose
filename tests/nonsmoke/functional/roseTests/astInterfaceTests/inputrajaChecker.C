// A simplest RAJA example to test RAJA checkers. 
//
// It contains cases to be warned about, including
// * use of data structure members within RAJA loops
//
// Liao, 3/16/2017
//#include "raja.h"
// Minimal RAJA header
#include <stdlib.h>
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

void foo_input()
{
  const int n = 15;

  struct ManagedArray{
    int m1;
  };

  ManagedArray ma;
  ma.m1 = 100;

  double *a = new double [15];

  RAJA::forall< class RAJA::seq_exec  > (0,n, [=] (int i)
      {
      a[i] = 0.5 + ma.m1;

      });

  free(a);
}

int main()
{
  foo_input();
  return 0;
} 
