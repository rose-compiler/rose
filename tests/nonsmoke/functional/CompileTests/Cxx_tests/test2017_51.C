
// This is required for clang version 10.0
#include<typeinfo>



namespace boost
{
namespace core
{
  // This fails for clang++ version 10.0, but passes for GNU (all versions, as best I can tell).
  // Original code: typedef std::type_info typeinfo;
  // Generated code: typedef class std::type_info typeinfo;
     typedef std::type_info typeinfo;

} 
} 

namespace boost
{
namespace detail
{
   typedef boost::core::typeinfo sp_typeinfo;

   class sp_counted_base {};

   template<class P, class D, class A> class sp_counted_impl_pda: public sp_counted_base {};

   class shared_count
      {
        private:

             sp_counted_base * pi_;

        public:

             template<class P, class D, class A> shared_count( P p, D d, A a ): pi_( 0 )
                {
                  typedef sp_counted_impl_pda<P, D, A> impl_type;

                  typedef typename A::template rebind< impl_type >::other A2;

                  A2 a2( a );

                  pi_ = a2.allocate( 1, static_cast< impl_type* >( 0 ) );
                }
      };

} 
} 


