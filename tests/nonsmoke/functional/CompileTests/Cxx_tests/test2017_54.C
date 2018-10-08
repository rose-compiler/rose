
class shared_count
   {
     public:
          template<class A> shared_count( A a )
             {
            // With "::other" this is longer running before the error, and without
            // it ends with what appears to be the same error but much faster.
            // typedef typename A::template rebind< A >::other A2;
               typedef typename A::template rebind< A >::other A2;

               A2 a2(a);

               a2.allocate();
             }
   };



