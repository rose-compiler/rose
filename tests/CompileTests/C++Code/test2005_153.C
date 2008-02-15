// Tests of namespaces a function qualifications

namespace A
   {
     void foo();
     namespace B
        {
          void foo();
          template <typename T> void foobar (T t);

       // end of namescope B
        }

  // error: function "A::B::foo" may not be redeclared in the current scope
  // void B::foo();

     class X
        {
          public:
               void foo() {};
               template <typename T> void foobar (T t) {}
        };

     template <typename T>
     class Y
        {
          public:
               void foo() {};
               template <typename S> void foobar (S s) {}
        };

  // end of namescope A
   }

void A::foo() {}
void A::B::foo() {}

template <typename T> void A::B::foobar (T t) {}
// template <typename T> void A::X::foobar (T t) {}

// void A::X::foo() {}

int main()
   {
     A::B::foobar(1);

     A::X x;
     x.foobar(1);

     A::Y<int> y;
     y.foo();
     y.foobar(1);

     return 0;
   }

