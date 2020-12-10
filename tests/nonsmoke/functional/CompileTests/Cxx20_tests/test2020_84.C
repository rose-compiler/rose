template<class T> void f(T, T*);    // #1
template<class T> void f(T, int*);  // #2

void m(int* p) {

 // DQ (7/21/2020): Failing case (ambiguous case) should not be tested
 // f(0, p); // deduction for #1: void f(T, T*) [T = int]
             // deduction for #2: void f(T, int*) [T = int]
 // partial ordering:
 // #1 from #2: void(T,T*) from void(U1,int*): P1=T, A1=U1: T=U1
 //                                            P2=T*, A2=int*: T=int: fails
 // #2 from #1: void(T,int*) from void(U1,U2*) : P1=T A1=U1: T=U1
 //                                              P2=int* A2=U2*: fails
 // neither is more specialized w.r.t T, the call is ambiguous
}

