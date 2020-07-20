template<int I, int J> A<I+J> f(A<I>, A<J>); // overload #1
template<int K, int L> A<K+L> f(A<K>, A<L>); // same as #1
template<int I, int J> A<I-J> f(A<I>, A<J>); // overload #2


template <int I, int J> void f(A<I+J>);  // template overload #1
template <int K, int L> void f(A<K+L>);  // equivalent to #1

