// This is a copy of test2005_172.C

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
              template <typename S> void foobar_Y (S s); // {}
            // void foobar_Y (int s) {}
        };

  // template<> void Y::foobar_Y (int s) {};
  // template<> void A::Y < int > ::foobar_Y(int s);
  // end of namescope A
   }

// It is not allowed to declare this in this way.  The use of name qualification is not an alternative to the specification of a namespace.
// template<> void A::Y<int>::foobar_Y (int s) {};
// namespace A {   template<> void Y<int>::foobar_Y (int s) {}; }

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
