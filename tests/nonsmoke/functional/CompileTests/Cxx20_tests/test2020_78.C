// equivalent
template <int I> void f(A<I>, A<I+10>); // overload #1
template <int I> void f(A<I>, A<I+10>); // redeclaration of overload #1
 
// not equivalent
template <int I> void f(A<I>, A<I+10>); // overload #1
template <int I> void f(A<I>, A<I+11>); // overload #2
 
// functionally-equivalent but not equivalent
// This program is ill-formed, no diagnostic required
template <int I> void f(A<I>, A<I+10>); // overload #1
template <int I> void f(A<I>, A<I+1+2+3+4>); // functionally equivalent


