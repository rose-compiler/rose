template<typename _Tp> 
class decay 
   {
     public:
          typedef _Tp type;
   };

namespace policy 
   {
     struct parstream {};
   }

template < typename my_policy, typename my_kernel >
struct ares_raja_api_kernel_type_check 
   {
     using kernel_type = typename decay<my_kernel>::type;
     static constexpr bool value = true;
   };

#if 1
template < typename my_policy, typename kernel_type >
inline void for_all_2d( kernel_type&& kernel )
   {
     static_assert( ares_raja_api_kernel_type_check<my_policy, kernel_type>::value, "xxx" );
   }
#endif

#if 0
void foobar()
   {
     int jp = 0;
     for_all_2d< policy::parstream > ( [=] (int i, int j) { int off = jp; });
   }
#endif
