#if 0
template<class T> class SwigValueWrapper
   {
  // T *tt;
     public:
       // inline SwigValueWrapper() : tt(0) { }
       // inline ~SwigValueWrapper() { if (tt) delete tt; }

       // Added explicit copy constructor (too make sure the correct version is called)
       // SwigValueWrapper (const SwigValueWrapper & t) { tt = t.t; }

       // inline SwigValueWrapper& operator=(const T& t) { tt = new T(t); return *this; }
       // inline operator T&() const { return *tt; }
       // inline T *operator&() { return tt; }
          operator T&() const;
   };
#endif

// This test code demonstrates an error in the unparsing of conversion operators
// (these are also refered to as casting operators).

template<class T> class SwigValueWrapper
   {
     public:
       // C++ casting operator definition
          operator T&() const;
   };

class X {};

template<class T> class Y
   {
     public:
         typedef int templateArgument;
   };

void foo()
   {
  // Case using non-nested template class (any class type)
     SwigValueWrapper< X > X_result;
     X *X_resultptr;

#if 1
  // DQ (9/5/2005): Commented out to allow tests with AST testing in place 9used to pass just fine!

  // Force the conversion operator to be called so that the copy constructor for X can be called with new!
     X_resultptr = new X ((X&)X_result);

  // Case using nested templates Y<Y<X> > does not compile because
  // EDG generates typename of "Y<Y<X>>" instead of "Y<Y<X> >"
     SwigValueWrapper< Y<Y<X> > > Y_result;
     Y<Y<X> > *Y_resultptr;

  // Force the conversion operator to be called so that the copy constructor for Y<Y<X>> can be called with new!
     Y_resultptr = new Y<Y<X> > ((Y<Y<X> >&)Y_result);
#endif
   }


