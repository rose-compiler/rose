template<class  T, class... U> void f(T, U...);           // #1
template<class  T            > void f(T);                 // #2
void h(int i) {
  f(&i);        // calls #2 due to the tie-breaker between parameter pack and no parameter
                // (note: was ambiguous between DR692 and DR1395)
}

