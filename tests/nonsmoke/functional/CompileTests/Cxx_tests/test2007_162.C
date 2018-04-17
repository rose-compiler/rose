// This is a copy related to test2005_172.C

// Tests of namespaces a function qualifications

// Skip version 4.x gnu compilers
#if ( __GNUC__ == 3 )

namespace A
   {
     template <typename T>
     class Y
        {
          public:
            // This template function must be defined in this template (to get the template nesting depth correct)
            // However if it is just a declaration, ROSE generated the wrong code!
              template <typename S> void foobar_Y (S s);// {}
        };
   }

int main()
   {
     A::Y<int> y;
  // A::Y y;
     y.foobar_Y(1);

     return 0;
   }

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif
