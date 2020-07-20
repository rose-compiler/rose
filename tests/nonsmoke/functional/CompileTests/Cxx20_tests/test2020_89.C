template<class T, class U> struct A { };
template<class T, class U> void f(U, A<U,T>* p = 0); // #1
template<         class U> void f(U, A<U,U>* p = 0); // #2
void h() {
  f<int>(42, (A<int, int>*)0);  // calls #2
  f<int>(42);                   // error: ambiguous
}

