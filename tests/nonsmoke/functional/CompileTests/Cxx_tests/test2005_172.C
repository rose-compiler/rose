// This is a simpler version of test2005_153.C

// Tests of namespaces a function qualifications

// Skip version 4.x gnu compilers
#if ( __GNUC__ == 3 )
namespace A
   {
     template <typename T>
     class Y
        {
          public:
               template <typename S> void foobar_Y (S s) {}
        };

  // end of namescope A
   }


int main()
   {
     A::Y<int> y;
     y.foobar_Y(1);

     return 0;
   }

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif
