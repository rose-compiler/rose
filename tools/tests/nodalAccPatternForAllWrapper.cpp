//Test case for nodal accumulation pattern
// using a wrapper function on top of RAJA::for_all() as a loop
// Liao, 2/22/2018
//
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

// a wrapper template function  here
template < typename EXEC_POLICY_T, typename LOOP_BODY > 
void for_all ( RAJA::Index_type begin, RAJA::Index_type end, LOOP_BODY loop_body)
{
  RAJA::forall <EXEC_POLICY_T> ( begin, end, loop_body );
}

// a wrapper template function 2 here
template < typename EXEC_POLICY_T, typename LOOP_BODY > 
void for_all_zones ( RAJA::Index_type begin, RAJA::Index_type end, LOOP_BODY loop_body)
{
  RAJA::forall <EXEC_POLICY_T> ( begin, end, loop_body );
}

// a wrapper template function 2 here
template < typename EXEC_POLICY_T, typename LOOP_BODY > 
void for_all_zones_tiled ( RAJA::Index_type begin, RAJA::Index_type end, LOOP_BODY loop_body)
{
  RAJA::forall <EXEC_POLICY_T> ( begin, end, loop_body );
}

template < typename EXEC_POLICY_T, typename LOOP_BODY > 
void for_all_clean_zones ( RAJA::Index_type begin, RAJA::Index_type end, LOOP_BODY loop_body)
{
  RAJA::forall <EXEC_POLICY_T> ( begin, end, loop_body );
}

template < typename EXEC_POLICY_T, typename LOOP_BODY > 
void for_allclean_zones ( RAJA::Index_type begin, RAJA::Index_type end, LOOP_BODY loop_body)
{
  RAJA::forall <EXEC_POLICY_T> ( begin, end, loop_body );
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
   for_all <class RAJA::seq_exec> (begin, end, [=](int i)
   {
      // Condition 4: accumulation pattern: lhs accum-op rhs
      // lhs : array element access x[i]: x is pointer type, i is loop index 
      // rhs: a scalar double type
      // accum-op:   +=, -=, *=, /=, MIN (), MAX() 
      x1[i] += rh1; 
      x2[i] -= rh1; 
      x3[i] *= rh1; 
      x4[i] /= rh1; 
   } );

   for_all_zones <class RAJA::seq_exec> (begin, end, [=](int i)
   {
      // Condition 4: accumulation pattern: lhs accum-op rhs
      // lhs : array element access x[i]: x is pointer type, i is loop index 
      // rhs: a scalar double type
      // accum-op:   +=, -=, *=, /=, MIN (), MAX() 
      x1[i] += rh1; 
      x2[i] -= rh1; 
      x3[i] *= rh1; 
      x4[i] /= rh1; 
   } );

   for_all_zones_tiled <class RAJA::seq_exec> (begin, end, [=](int i)
   {
      // Condition 4: accumulation pattern: lhs accum-op rhs
      // lhs : array element access x[i]: x is pointer type, i is loop index 
      // rhs: a scalar double type
      // accum-op:   +=, -=, *=, /=, MIN (), MAX() 
      x1[i] += rh1; 
      x2[i] -= rh1; 
      x3[i] *= rh1; 
      x4[i] /= rh1; 
   } );

   for_all_clean_zones <class RAJA::seq_exec> (begin, end, [=](int i)
   {
      // Condition 4: accumulation pattern: lhs accum-op rhs
      // lhs : array element access x[i]: x is pointer type, i is loop index 
      // rhs: a scalar double type
      // accum-op:   +=, -=, *=, /=, MIN (), MAX() 
      x1[i] += rh1; 
      x2[i] -= rh1; 
      x3[i] *= rh1; 
      x4[i] /= rh1; 
   } );

   for_allclean_zones <class RAJA::seq_exec> (begin, end, [=](int i)
   {
      // Condition 4: accumulation pattern: lhs accum-op rhs
      // lhs : array element access x[i]: x is pointer type, i is loop index 
      // rhs: a scalar double type
      // accum-op:   +=, -=, *=, /=, MIN (), MAX() 
      x1[i] += rh1; 
      x2[i] -= rh1; 
      x3[i] *= rh1; 
      x4[i] /= rh1; 
   } );

}

int main()
{
  //foo();                                                                                                        
  return 0;                                                                                                           
}
