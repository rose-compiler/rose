
#if 0
template<class T, class U> struct X { void f() { } };

template<class T> struct X<T, T*> { void f() { } };
#else
template<class T, class U> struct X { };
template<class T> struct X<T, T*> { };
#endif

#if 0
void foobar() 
   {
//   X<int, int> a;
     X<int, int*> b;
//   a.f(); 
     b.f();
   }
#endif

