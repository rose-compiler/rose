
template<class T> class X;

template<class T> X<T> *g(X<T>);

template<class T> 
class X
   {
     public:
          X(T ii, T jj) : i(ii), j(jj) { }
          operator T() { return i + j; }
          friend int f(void *, int);
          friend X<T> *g<>(X<T>);
     private:
          T i, j;
   };

int f(void *p, int i)
   {
     switch (i)
        {
          case 0:
             {
               X<short> *ps = (X<short>*)p;
               return ps->i;
             }
          case 1:	
             {
               X<int> *pi = (X<int>*)p;
               return pi->i;
             }
          case 2:	
             {
               X<long> *pl = (X<long>*)p;
               return int(pl->i);
             }
        }

     return 0;
   }

template<class T> X<T> *g(X<T> r)
   {
     return new X<T> (r.i + 1, r.j + 1);
   }

void foobar()
   {
     X<short> si(1, 1);
     X<int> i(2, 2);
     X<long> li(4, 4);
     f(&si, 0);
     f(&i, 1);
     f(&li, 2);

     X<short> *ps = g(si);
     *ps;
     delete ps;
     X<int> *pi = g(i);
     *pi;
     delete pi;
     X<long> *pl = g(li);
     *pl;
     delete pl;
   }

#if 0
// Same error for all three: error: specializing member ‘::g<short int>’ requires ‘template<>’ syntax

// Also, the error is that the template instantiations are output.
class X< short  > *g< short  > (class X< short  > r)
{
  return new class X< short  > ((r . i + 1),(r . j + 1));
}

class X< int  > *g< int  > (class X< int  > r)
{
  return new class X< int  > (r . i + 1,r . j + 1);
}

class X< long  > *g< long  > (class X< long  > r)
{
  return new class X< long  > (r . i + 1,r . j + 1);
}
#endif
