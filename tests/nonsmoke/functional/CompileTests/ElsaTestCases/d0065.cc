// testing integer arithmetic in template parameters
template<class T, int I> struct A;
template<class T, int I> T g(A<T*, I+1> a, T i[I]);
//  template<class T, int I> T g(A<T*, I+1> a, T i[I]) {
//    return i[0];
//  }

// try instantiating it as well
