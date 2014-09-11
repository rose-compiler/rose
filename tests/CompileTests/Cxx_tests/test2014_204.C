template <typename T>
class X
   {
     public: 
          class iterator {};

          iterator foo();
   };

// template X<int>::iterator X<int>::foo();

void foobar()
   {
     X<int> x;
     x.foo();
   }

#if ( (__GNUC__ == 4) && (__GNUC_MINOR__ >= 4) )
// GNU g++ version 4.4.7 does not like this (EDG compiles it just fine).
// template < typename T> X< T> ::iterator X< T> ::foo() { }
template <typename T> 
// Note that we require typename for the GNU g++ compiler (not for EDG).
// This was a confusing point in the error message from g++ version 4.4.7, 
// but g++ version 4.8.1 error message was much more clear that typename 
// was missing.  But "typename" is not required for EDG.
typename X<T>::iterator
X<T>::foo()
   {
   }
#endif
