// Tests of namespaces a function qualifications

// Skip version 4.x gnu compilers
#if ( __GNUC__ == 3 )

namespace A
   {
     void foo();
     namespace B
        {
          void foo();
          template <typename T> void foobar_B (T t);

       // end of namescope B
        }

  // error: function "A::B::foo" may not be redeclared in the current scope
  // void B::foo();

     class X
        {
          public:
               void foo() {};
               template <typename T> void foobar_X (T t) {}
        };

     template <typename T>
     class Y
        {
          public:
               void foo() {};
               template <typename S> void foobar_Y (S s) {}
        };

  // end of namescope A
   }

void A::foo() {}
void A::B::foo() {}

template <typename T> void A::B::foobar_B (T t) {}
// template <typename T> void A::X::foobar (T t) {}

// void A::X::foo() {}

int main()
   {
     A::B::foobar_B(1);

     A::X x;
     x.foobar_X(1);

     A::Y<int> y;
     y.foo();
     y.foobar_Y(1);

     return 0;
   }

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

