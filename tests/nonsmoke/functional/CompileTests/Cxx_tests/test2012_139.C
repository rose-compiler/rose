//
// Main program illustrating basic index set creation and traversal.
//

#include <cstdlib>

#include<string>
#include<iostream>

#define USE_ICC
//#define USE_XLC


/*!
 ******************************************************************************
 *
 * \brief  Simple range traversal template method.
 *
 ******************************************************************************
 */
template <typename LOOP_BODY>
void IndexSet_forall(unsigned begin, unsigned length, LOOP_BODY loop_body)
{
   for ( unsigned ii = 0 ; ii < length ; ++ii ) {
      loop_body( ii+begin );
   }
}


/*!
 ******************************************************************************
 *
 * \brief  Function to check result. 
 *
 ******************************************************************************
 */
void ResultCheck(const std::string& name,
                 double* ref_result,  
                 double* to_check,  
                 unsigned iset_len)
{
   bool is_correct = true;
   for (unsigned i = 0 ; i < iset_len; ++i) {
      is_correct &= ref_result[i] == to_check[i];
   }
   
   std::cout << name << "is " 
             << (is_correct ? "CORRECT" : "WRONG") << std::endl; 
}


/*!
 ******************************************************************************
 *
 * \brief  Function to check result. 
 *
 ******************************************************************************
 */
class TestOp
{
public:
   TestOp(double* __restrict__ parent, double* __restrict__ child) :
      m_parent(parent), m_child(child) { ; }

   void operator() (unsigned idx)
   {
#if defined(USE_ICC) // if using Intel compiler (icc)
      __assume_aligned(m_parent, 32) ;
      __assume_aligned(m_child, 32) ;
#endif
#if defined(USE_XLC) // if using IBM compiler (xlcxx)
      __alignx(32, m_parent) ;
      __alignx(32, m_child) ; 
#endif
      m_child[idx] = m_parent[idx] * m_parent[idx];
   }

   double* __restrict__ m_parent;
   double* __restrict__ m_child;
};


int main(int argc, char *argv[])
{
   //
   // Allocate and initialize arrays for tests...
   //
   const unsigned array_length = 320;

   double* parent;
   double* child;
   double* child_ref;
   posix_memalign((void **)&parent, 32, array_length*sizeof(double)) ;
   posix_memalign((void **)&child, 32, array_length*sizeof(double)) ;
   posix_memalign((void **)&child_ref, 32, array_length*sizeof(double)) ;

   for (int i=0 ; i<array_length; ++i) {
      parent[i] = (double) (rand() % 65536) ;
      child[i] = 0.0;
      child_ref[i] = 0.0;
   }

   //
   // Generate reference result to check correctness
   //

#if defined(USE_ICC) // if using Intel compiler (icc)
   auto ref_op = [&] (int idx) { __assume_aligned(parent, 32) ;
                                 __assume_aligned(child_ref, 32) ;
                                 child_ref[idx] = parent[idx] * parent[idx]; };
#endif

#if defined(USE_XLC) // if using IBM compiler (xlcxx)
   TestOp ref_op(parent, child_ref);
#endif

   // Execute full array traversal as correct result...
   IndexSet_forall(0, array_length, ref_op);

#if defined(USE_ICC)
#if 0  // use lambda
   auto test_op = [&] (int idx) { __assume_aligned(parent, 32) ;
                                  __assume_aligned(child, 32) ;
                                  child[idx] = parent[idx] * parent[idx]; };
#else  // use functor
   TestOp test_op(parent, child);
#endif
#endif

#if defined(USE_XLC) // if using IBM compiler (xlcxx)
   TestOp test_op(parent, child);
#endif


   // Execute half array traversal and check result...
   IndexSet_forall(0, array_length/2, test_op);
   ResultCheck("half array ",
               child_ref,  
               child,  
               array_length/2);

#if 0
   for (int ic = 0; ic < 320; ++ic) {
      std::cout << "child[" << ic << "] = " << child[ic] << std::endl; ;
   }
#endif

   free(parent);
   free(child);
   free(child_ref);

   std::cout << "\n DONE!!! " << std::endl;

   return 0 ;
}

