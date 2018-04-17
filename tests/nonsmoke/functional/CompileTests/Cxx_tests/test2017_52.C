
template<class P, class D, class A> class sp_counted_impl_pda {};

class shared_count
   {
     public:
          template<class P, class D, class A> shared_count( P p, D d, A a )
             {
               typedef sp_counted_impl_pda<P, D, A> impl_type;
               typedef typename A::template rebind< impl_type >::other A2;

               A2 a2( a );

               a2.allocate();
             }
   };



