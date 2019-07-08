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
     new B<C>(&C::foobar, this);
   }

