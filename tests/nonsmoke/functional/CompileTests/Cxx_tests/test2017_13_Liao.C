// undefined function call in a template function, testing KeepGoingTranslator
// Strangely, GCC and Clang won't complain.
// There are some relevant discussion at:
//  http://stackoverflow.com/questions/20599890/can-you-use-an-undefined-type-in-a-c-template-function-if-the-function-is-neve
//
// Liao, 4/21/2017

namespace loop_over {
 struct index {};
}

template < typename policy,                                                                                                           
           typename index_type,                                                                                                       
           typename kernel_type >                                                                                                     
inline void for_all( loop_over::index,                                                                                                
                     const index_type* ndx_array,                                                                                     
                     const index_type& begin,                                                                                         
                     const index_type& end,                                                                                           
                     kernel_type kernel )                                                                                             
{
  ARES_ASSERT_ERROR( begin <= end );                                                                                                  
  ARES_ASSERT_ERROR( ndx_array != __null );                                                                                           

} 

