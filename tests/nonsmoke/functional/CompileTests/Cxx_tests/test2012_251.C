template<class T> class SwigValueWrapper
   {
     public:
       // C++ casting operator definition
          operator T&() const;
   };

class X {};

void foo()
   {
  // Case using non-nested template class (any class type)
     SwigValueWrapper< X > X_result;
     X *X_resultptr;

  // Force the conversion operator to be called so that the copy constructor for X can be called with new!
     X_resultptr = new X ((X&)X_result);
   }


