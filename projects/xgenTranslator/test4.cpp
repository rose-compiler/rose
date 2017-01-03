// Liao 9/2/2016
// Not in use for now
// An example showing the generated code
// used for template function body generation reference
//#include <stdlib.h>

// Simplest input and output code for as example
namespace RAJA {

  typedef int Index_type;

  // some prebuilt policies
  struct seq_exec {};  // the type for specialization
//  struct omp_parallel_for_exec {};

  // input code , template 1
  // Using pragma to specify which template to specialize for now
#pragma xgen specialize_template
  template <typename EXEC_POLICY_T,
           typename LOOP_BODY>
             //             inline __attribute__((always_inline))
             void forall(int begin, int end,  //SgTemplateFunctionDeclaration
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
      //      omp_parallel_for_exec,
      NUM_POLICIES
    };

    POLICY_TYPE
      getRandomPolicy() {
        return seq_exec;
      }
  } // end switcher namespace

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

  // This is the new specialization we want to generate
  struct switcher_exec{}; 
  template <typename LOOP_BODY>
    void forall(switcher_exec,
        int begin, int end,
        LOOP_BODY loop_body)                                                                                                
    {                                                                                                                       
      switch(switcher::getRandomPolicy()) {                                                                                 
        case switcher::seq_exec:                                                                                            
          RAJA::forall( RAJA::seq_exec(), begin, end, loop_body ); break;                                                   
//        case switcher::omp_parallel_for_exec:                                                                               
//          RAJA::forall( RAJA::omp_parallel_for_exec(), begin, end, loop_body ); break;                                      
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

