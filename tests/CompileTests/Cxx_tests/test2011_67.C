namespace N
   {
     class any {};

     template < typename T, typename S = any >
     class array
        {
          public:
               array();
            // array(int x);

               void foobar();
               void foo()
                  {
                    array<T> X;
                  }
        };
   }


template < typename T, typename S >
void N::array<T,S>::foobar()
   {
     array<T> X;
   }

void foo()
   {
     N::array<N::any> A;

  // N::array<N::any,N::any>::foobar();
   }

