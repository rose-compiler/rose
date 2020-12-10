template<class T> void f(T);        // template #1
template<class T> void f(T*);       // template #2
template<class T> void f(const T*); // template #3
void m() {
  const int* p;
  f(p); // overload resolution picks: #1: void f(T ) [T = const int *]
        //                            #2: void f(T*) [T = const int]
        //                            #3: void f(const T *) [T = int]
// partial ordering
// #1 from transformed #2: void(T) from void(U1*): P=T A=U1*: deduction ok: T=U1*
// #2 from transformed #1: void(T*) from void(U1): P=T* A=U1: deduction fails
// #2 is more specialized than #1 with regards to T
// #1 from transformed #3: void(T) from void(const U1*): P=T, A=const U1*: ok
// #3 from transformed #1: void(const T*) from void(U1): P=const T*, A=U1: fails
// #3 is more specialized than #1 with regards to T
// #2 from transformed #3: void(T*) from void(const U1*): P=T* A=const U1*: ok
// #3 from transformed #2: void(const T*) from void(U1*): P=const T* A=U1*: fails
// #3 is more specialized than #2 with regards to T
// result: #3 is selected
// in other words, f(const T*) is more specialized than f(T) or f(T*)
}

