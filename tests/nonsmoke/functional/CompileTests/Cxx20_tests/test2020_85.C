template<class T> void g(T);  // template #1
template<class T> void g(T&); // template #2
void m() {
  float x;
  g(x); // deduction from #1: void g(T ) [T = float]
        // deduction from #2: void g(T&) [T = float]
// partial ordering
// #1 from #2: void(T) from void(U1&): P=T, A=U1 (after adjustment), ok
// #2 from #1: void(T&) from void(U1): P=T (after adjustment), A=U1: ok
// neither is more specialized w.r.t T, the call is ambiguous
}

