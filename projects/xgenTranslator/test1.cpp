// Liao 9/1/2016
// Simplest input code for as example
#include <stdlib.h>

namespace RAJA 
{
  typedef int Index_type;

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

  // the underneath basic specialization should be provided in the input code
  struct seq_exec {};  // the type for specialization
  struct omp_parallel_for_exec {};  // the type for specialization

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

  // output code, template 1 specialization  
#if 0 // This is the specialization we want to create
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

} // end namespace of RAJA

#if 1
int main()
{
  const int n =15; 
  double* a = new double [n];
//  double* b = new double [n];

  RAJA::forall<RAJA::seq_exec>(0, n, 
      [=] (int i) {  // mimic the possible input file
        a[i] = 0.5;
      });


  return 0; 
}

#endif
// g++ -c -std=c++11 
// identityTranslator -rose:Cxx11_only

