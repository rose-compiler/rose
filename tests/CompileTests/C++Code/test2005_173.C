
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

void foo()
   {
     X1<X> x;
   }

