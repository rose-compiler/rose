
template<class T> class X;

template<class T> void foo(X<T>);

template<class T> 
class X
   {
     public:
          friend void foo<>(X<T>);

     private:
          T i, j;
   };

template<class T> void foo(X<T> r)
   {
   }

void foobar()
   {
     X<short> si;

     foo(si);
   }
