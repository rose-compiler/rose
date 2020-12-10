// Simplest RAJA example code
// to explore the feasible of building a ROSE-based RAJA lowering tool.
//
// Liao, 3/27/2019
// Updated in /4/3/2020: elaborate instantiation and translation steps. 
// Compile this code
//    g++ -std=c++11  // GCC 4.9.x is tested.
//----------- begin of minimal jara.h
namespace RAJA
{
  typedef int Index_type;
  // new specialized policy
  // template 1
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
  // template 2: some prebuilt specialization for sequential and parallel executions
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
 //------------------end of min raja.h--------
 // RAJA code
 // --------------------------------
 void foo_input()
{
  const int n = 15;
  double *a = new double [15];

  // This is the input raja::forall loop to be translated
  // Original raja template function call
  RAJA::forall< class RAJA::seq_exec  > (0,n, [=] (int i)
      {
      a[i] = 0.5;
      });

  // First stage of translation (this may already happen in ROSE frontend as part of template instantiation): 
  // Input: raja template function call deflined at line 57, 
  //        instantiated using template 1 defined at line 15:
  // Output should look like:
  RAJA::forall(RAJA::seq_exec() , 0, n,  [=] (int i)
      {
      a[i] = 0.5;
      }   );

  // Second stage of translation: specialize input further using template 2 defined at line 32-43
  // Input: instantiated raja::forall defined at line 66-69
  // Translation: peel off the function header, keep the function body only
  //    essentially inlining the template 2 
  // Output: the following code based on the body of template 2

  // generate a named lambda expression: a functor
  auto loop_body_1 =   [=] (int i)  {   a[i] = 0.5;  };  
  // inline the body of template 2: the types are instantiated
  {
    ; 
#pragma novector
    for ( int i = 0; i < n; ++ i ) {
      loop_body_1 ( i );
    }
    ;
  } 

  // Third stage of translation: inlining lambda at line 78 into the instantiated template 2 body at call site line 84
  //Final translation output
  {
    ; 
#pragma novector
    for (int i=0; i<n; i++ )
    {
      a[i]=0.5;
    }
    ;
  }

  // Fourth stage of translation: cleanup the code , remove unnecessary scope, empty statements, etc
  // This is what users really want to have in the end: nice, clean loops:
#pragma novector
  for (int i=0; i<n; i++ )
  {
    a[i]=0.5;
  }

  // not calling this to avoid including a header:   free(a);
}

 int main()
{
  foo_input();
  return 0;
}

