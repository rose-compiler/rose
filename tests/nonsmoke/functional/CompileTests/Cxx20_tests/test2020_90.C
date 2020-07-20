template<class T           >  void g(T, T = T());  // #1
template<class T, class... U> void g(T, U ...);    // #2
void h() {
  g(42);  // error: ambiguous
}

