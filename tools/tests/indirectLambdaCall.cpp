//Test case for nodal accumulation pattern
// using RAJA::for_all() as a loop
// separated kernel declaration and calling
namespace RAJA
{
  typedef int Index_type;
  // new specialized policy

 // input code , template 1
  template < typename EXEC_POLICY_T, typename LOOP_BODY >
    void forall ( Index_type begin, Index_type end, LOOP_BODY loop_body)
    {
      forall ( EXEC_POLICY_T(), begin, end, loop_body );
    }
  // some prebuilt policies
  // the type for specialization

  struct seq_exec
  {
  }
  ;
  // Some prebuilt specialization for sequential and parallel executions
  template < typename LOOP_BODY >
    void forall ( seq_exec, Index_type begin, Index_type end, LOOP_BODY loop_body )
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


void foo(double* x, int jp, int kp, RAJA::Index_type begin, RAJA::Index_type end, double rh1)
{
   //Condition 1: pointer declaration, 4 or 8 pointers
   double * x1, *x2, *x3, *x4; 

   //Condition 2:  pointer initialization, using other pointers on rhs
   x1 = x;
   x2 = x +1; 
   x3 = x1 + jp; 
   x4 = x1 + kp; 

   //Condition 3:  A regular loop or a RAJA loop
   // Declare kernel here
  auto kernel = [=](int i)
   {
      // Condition 4: accumulation pattern: lhs accum-op rhs
      // lhs : array element access x[i]: x is pointer type, i is loop index 
      // rhs: a scalar double type
      // accum-op:   +=, -=, *=, /=, MIN (), MAX() 
      x1[i] += rh1;
      x2[i] -= rh1;
      x3[i] *= rh1;
      x4[i] /= rh1;
   } ; 
  
  // called within a RAJA::forall() function
   RAJA::forall <class RAJA::seq_exec> (begin, end, kernel);
}

