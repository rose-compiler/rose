template<class T1, class T2, int I>
class A {}; // primary template
 
template<class T, int I>
class A<T, T*, I> {}; // partial specialization where T2 is a pointer to T1
 
template<class T, class T2, int I>
class A<T*, T2, I> {}; // partial specialization where T1 is a pointer
 
template<class T>
class A<int, T*, 5> {}; // partial specialization where T1 is int, I is 5,
                        // and T2 is a pointer
 
template<class X, class T, int I>
class A<X, T*, I> {}; // partial specialization where T2 is a pointer
