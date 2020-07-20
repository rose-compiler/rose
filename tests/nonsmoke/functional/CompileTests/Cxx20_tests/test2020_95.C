template <class T>          T f(int);  // #1
template <class T, class U> T f(U);    // #2
void g() {
  f<int>(1);  // specialization of #1 is explicit: T f(int) [T = int]
              // specialization of #2 is deduced:  T f(U) [T = int, U = int]
// partial ordering (only considering the argument type)
// #1 from #2: T(int) from U1(U2): fails
// #2 from #1: T(U) from U1(int): ok: U=int, T unused
// calls #1
}

