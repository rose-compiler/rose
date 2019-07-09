// This is one of three ROSE bugs specific to the SL application.
// This is a case of the "class" specifier being output in the 
// wrong location (should not be output).

class A {};

template <class T>
class B
   {
     typedef void (T::*XXX)(A);
     public:
          B( XXX a, T* b) {}
   };

class C
   {
     protected: 
          C();
     private:
          void foobar (A x);
   };

C::C()
   {
  // original code: new B<C>(&C::foobar, this);
  // generated code: new B<C>(&class C::foobar, this);
     new B<C>(&C::foobar, this);
   }

