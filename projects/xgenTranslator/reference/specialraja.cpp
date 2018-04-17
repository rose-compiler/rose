#include "raja.h"

namespace RAJA 
{

 // big chunk of specialization to be generated
 //  struct switcher_exec{}; // moved into raja.h

  template <typename LOOP_BODY>
    void forall(switcher_exec,
        Index_type begin, Index_type end,
        LOOP_BODY loop_body)
    {
      switch(switcher::getRandomPolicy()) {
        case switcher::seq_exec:
          RAJA::forall( RAJA::seq_exec(), begin, end, loop_body ); break;
        case switcher::omp_parallel_for_exec:
          RAJA::forall( RAJA::omp_parallel_for_exec(), begin, end, loop_body ); break;
      }
    }
}

// We have to capture all variables, and transfer values
void outline_loop_1(int i, int j, double* a)
{
   RAJA::forall<RAJA::switcher_exec>
    (i, j,
     [=] (int k) {  // mimic the possible input file
     a[k] = 0.5;
     });
}

// g++ -std=c++11 -fpic -shared specialraja.cpp  -o libspecialraja.so
