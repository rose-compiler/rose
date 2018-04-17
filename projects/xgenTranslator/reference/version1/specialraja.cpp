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
#if 0
  // explicit instantiation of the specialization above, using a declaration
  template <> 
    void forall< > (switcher_exec, 0,10, [=](int i){} ); 
    //void forall<typename switcher_exec > ( 0,10, [=](int i){} ); 
#endif

   void foo ()
   {
    // explicit instantiation of the specialization above, using a call similar to the example code
    forall<switcher_exec > (0,10, [=](int i){} ); 
   }
}


// must do this so compilers will see implementations and put them into the shared library
//template // <> 
//void RAJA::forall<RAJA::switcher_exec > (0,10, [=](int i){} ); // explicit instantiation using a declaration



// g++ -std=c++11 -fpic -shared specialraja.cpp  -o specialraja.so
