template<class T> struct A { A(); };
 
template<class T> void h(const T&); // #1
template<class T> void h(A<T>&);    // #2
void m() {
  A<int> z;
  h(z);  // deduction from #1: void h(const T &) [T = A<int>]
         // deduction from #2: void h(A<T> &) [T = int]
 // partial ordering
 // #1 from #2: void(const T&) from void(A<U1>&): P=T A=A<U1>: ok T=A<U1>
 // #2 from #1: void(A<T>&) from void(const U1&): P=A<T> A=const U1: fails
 // #2 is more specialized than #1 w.r.t T
 
  const A<int> z2;
  h(z2); // deduction from #1: void h(const T&) [T = A<int>]
         // deduction from #2: void h(A<T>&) [T = int], but substitution fails
 // only one overload to choose from, partial ordering not tried, #1 is called
}

