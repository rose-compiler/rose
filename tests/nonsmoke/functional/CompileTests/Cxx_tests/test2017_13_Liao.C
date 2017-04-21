// undefined macros in a template function, testing KeepGoingTranslator
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

