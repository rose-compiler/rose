template<class  T, class... U> void g(T*, U...);          // #1
template<class  T            > void g(T);                 // #2
void h(int i) {
  g(&i);        // OK: calls #1 (T* is more specialized than T)
}

