// Liao 9/30/2016
// Minimal RAJA header
#include <stdlib.h>

// Simplest input and output code for as example
namespace RAJA 
{

  typedef int Index_type;

  // new specialized policy
  struct switcher_exec{};

  // input code , template 1
  // Using pragma to specify which template to specialize for now
#pragma xgen specialize_template
  template <typename EXEC_POLICY_T,
           typename LOOP_BODY>
             void forall(Index_type begin, Index_type end,
                 LOOP_BODY loop_body)
             {
               forall( EXEC_POLICY_T(),
                   begin, end,
                   loop_body );
             }

  // some prebuilt policies
  struct seq_exec {};  // the type for specialization
  struct omp_parallel_for_exec {};
  struct omp_for_nowait_exec {};
  struct omp_parallel_for_segit {};
  struct omp_parallel_segit {};
  struct omp_taskgraph_segit {};
  struct omp_taskgraph_interval_segit {};
  struct omp_reduce {};


  // Some prebuilt specialization for sequential and parallel executions
  template <typename LOOP_BODY>
    void forall(seq_exec,
        Index_type begin, Index_type end,                                                                                         
        LOOP_BODY loop_body)                                                                                                      
    {                                                                                                                             
      ;                                                                                                                           
#pragma novector                                                                                                                  
      for ( Index_type ii = begin ; ii < end ; ++ii ) {                                                                           
        loop_body( ii );                                                                                                          
      }                                                                                                                           
      ;                                                                                                                           
    }

  template <typename LOOP_BODY>
    void forall(omp_parallel_for_exec,
        Index_type begin, Index_type end,
        LOOP_BODY loop_body)
    {
      ;
#pragma omp parallel for schedule(static)
      for ( Index_type ii = begin ; ii < end ; ++ii ) {
        loop_body( ii );
      }
      ;
    }

  // We can assume this sub namespace is provided as input , including a list of execution policies
  namespace switcher {
    enum POLICY_TYPE {
      seq_exec = 0,
      omp_parallel_for_exec,
      NUM_POLICIES
    };

    inline __attribute__((always_inline))
      POLICY_TYPE
      getRandomPolicy() {
        int coin_toss = rand() % 2;
        if (coin_toss >= 1) {
          return seq_exec;
        } else {
          return omp_parallel_for_exec;
        }
      }
  } // end switcher namespace

#if 0  // things we want to put into a library
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
#endif

} // end namespace

// g++ -c -std=c++11 
// identityTranslator -rose:Cxx11_only

