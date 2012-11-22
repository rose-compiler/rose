template<class T> class SwigValueWrapper
   {
     public:
       // C++ casting operator definition
          operator T&() const;
   };

class X {};

#if 1
template<class T> class Y
   {
     public:
         typedef int templateArgument;
   };
#endif

void foo()
   {
  // Case using non-nested template class (any class type)
     SwigValueWrapper< X > X_result;
     X *X_resultptr;

  // DQ (9/5/2005): Commented out to allow tests with AST testing in place 9used to pass just fine!

  // Force the conversion operator to be called so that the copy constructor for X can be called with new!
     X_resultptr = new X ((X&)X_result);

#if 1
  // Case using nested templates Y<Y<X> > does not compile because
  // EDG generates typename of "Y<Y<X>>" instead of "Y<Y<X> >"
     SwigValueWrapper< Y<Y<X> > > Y_result;
     Y<Y<X> > *Y_resultptr;

  // Force the conversion operator to be called so that the copy constructor for Y<Y<X>> can be called with new!
     Y_resultptr = new Y<Y<X> > ((Y<Y<X> >&)Y_result);
#endif
   }
