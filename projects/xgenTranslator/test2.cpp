// Liao 9/1/2016
// A complete example showing the generated code
#include <stdlib.h>

// Simplest input and output code for as example
namespace RAJA {

  typedef int Index_type;

  // some prebuilt policies
  struct omp_parallel_for_exec {};
  struct omp_for_nowait_exec {};
  struct omp_parallel_for_segit {};
  struct omp_parallel_segit {};
  struct omp_taskgraph_segit {};
  struct omp_taskgraph_interval_segit {};
  struct omp_reduce {};

  // input code , template 1
  // Using pragma to specify which template to specialize for now
#pragma xgen specialize_template
  template <typename EXEC_POLICY_T,
           typename LOOP_BODY>
             //             inline __attribute__((always_inline))
             void forall(Index_type begin, Index_type end,  //SgTemplateFunctionDeclaration
                 LOOP_BODY loop_body)
             {
               forall( EXEC_POLICY_T(),
                   begin, end,
                   loop_body );
             }


  // prebuilt specialization
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
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

  struct seq_exec {};  // the type for specialization
  template <typename LOOP_BODY>
    //    inline __attribute__((always_inline))
    void forall(seq_exec,                     // SgTemplateFunctionDeclaration
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
#if 0
    // This is only used for C++14 version
    template <typename BODY>
      inline __attribute__((always_inline))
      void policySwitcher(POLICY_TYPE policy, BODY body) {
        switch (policy) {
          case seq_exec: body(RAJA::seq_exec()); break;
          case omp_parallel_for_exec: body(RAJA::omp_parallel_for_exec()); break;
        }
      }
#endif
  } // end switcher namespace

  // This is the new specialization we want to generate
  struct switcher_exec{}; 
  template <typename LOOP_BODY>
    //    inline __attribute__((always_inline))
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
} // end namespace

int main()
{
  const int n =15; 
  double* a = new double [n];
  //  double* b = new double [n];

  //  RAJA::forall<RAJA::seq_exec> // the original policy is replaced with new switcher policy
  RAJA::forall<RAJA::switcher_exec>
    (0, n, 
     [=] (int i) {  // mimic the possible input file
     a[i] = 0.5;
     });


  return 0; 
}

// g++ -c -std=c++11 
// identityTranslator -rose:Cxx11_only

