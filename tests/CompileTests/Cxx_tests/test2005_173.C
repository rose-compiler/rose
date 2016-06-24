
class Y;

class X
   {
  // friend void Y::foobar();
   };

template<class T>
class Y1
   {
     void foobar();
   };

template<class T>
class X1
   {
     friend void Y1<T>::foobar();
   };

// Skip version 4.x gnu compilers
// #if ( __GNUC__ == 3 )
#if ( defined(__clang__) == 0 && __GNUC__ == 3)

void foo()
   {
     X1<X> x;
   }

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

