template<class T, class U, int I> struct X { void f() { } };

template<class T, int I> struct X<T, T*, I> { void f() { } };

int main() 
   {
     X<int, int, 10> a;
     X<int, int*, 5> b;
     a.f(); 
     b.f();
   }

