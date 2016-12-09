// Use this file for a test code!

/*
  template <class _T1, class _T2>
    inline void
    _Construct(_T1* __p, const _T2& __value)
    { new (static_cast<void*>(__p)) _T1(__value); }
*/

/* Initialization by constructor.  The forms are as follows:
       parenthesized_init
       TRUE                        (arg1, arg2, ...)
       FALSE copy constructor      arg1          <-- cctor elision case
             not copy constructor  T(arg1, arg2, ...)
*/

#if 1
// Demonstrates "not copy constructor", but not a reproduction of the case that failes in Kull!
// DQ (9/18/2004): I believe that the purpose of recognizing initialization by constructor 
// is to optimize away the redundand calls to the constructors (allowed by C++ standard).
class X
   {
     public:
          X( int y);
          X( const X & x );
   };

void foo()
   {
     int n = 1;
     X e(1);
   }
#endif

#if 0
template <class T>
class X
   {
     public:
          X( int y);
          X( const T & x );
   };

template <class T>
void foo(T t)
   {
     int n = 1;
     X<X> e(1);
   }

int main()
   {
     foo(5);
   }
#endif
