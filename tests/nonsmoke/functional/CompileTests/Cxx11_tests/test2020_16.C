
template<class T> class X;

template<class T> X<T> *g(X<T>);

template<class T> 
class X
   {
     public:
          friend X<T>* g<>(X<T>);

     private:
          T i, j;
   };

template<class T> X<T>* g(X<T> r)
   {
     return 0L;
   }

void foobar()
   {
     X<short> si;

     X<short> *ps = g(si);
   }

#if 0
// Same error for all three: error: specializing member ‘::g<short int>’ requires ‘template<>’ syntax

// Also, the error is that the template instantiations are output.
class X< short  > *g< short  > (class X< short  > r)
{
  return new class X< short  > ((r . i + 1),(r . j + 1));
}

#endif
