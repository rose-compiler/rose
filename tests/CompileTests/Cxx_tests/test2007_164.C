// This test code correctly translates to generated code where the
// template specialization prototype is not put into the namespace
// instead of global scope.  It might be that later versions of
// g++ require the function definition (for the template specialication)
// to also appear in the namespace (but this is not required for g++ 3.4.x).

namespace A
   {
     template <typename T>
     class Y
        {
          public:
            // template <typename S> void foobar_Y (S s) {}
               void foobar_Y (int s) {}
        };

  // end of namescope A
   }


int main()
   {
     A::Y<int> y;
     y.foobar_Y(1);

     return 0;
   }
