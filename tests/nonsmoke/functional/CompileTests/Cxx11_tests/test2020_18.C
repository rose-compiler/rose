template<class T> class X;
template<class T> void foo(X<T>);

template<class T> 
class X
   {
     public:
          friend void foo<>(X<T>);
   };

template<class T> void foo(X<T> r)
   {
   }

#if 0
template<>
void foo< short  > (class X< short  > r)
{
}
#endif

void foobar()
   {
     X<short> si;

     foo(si);
   }
