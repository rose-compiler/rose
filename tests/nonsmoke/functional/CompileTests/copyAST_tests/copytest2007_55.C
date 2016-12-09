// test2005_168.C

// This is a simpler version of test2005_153.C

// This testcode demonstrates that the generated prototype declaration 
// for a specialization need to appear in the correct namespace.

// Skip version 4.x gnu compilers
#if ( __GNUC__ == 3 )

namespace A
   {
     class X
        {
          public:
               template <typename T> void foobar (T t) {}
        };

  // end of namescope A
   }

int main()
   {
     A::X x;
     x.foobar(1);

     return 0;
   }

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

