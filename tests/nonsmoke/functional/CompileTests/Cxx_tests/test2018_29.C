
// template<class T, int N> class foobar {};

// template<class T, int A>
// int foo(T const(&)[N]) { return N; }

template<class T, int N>
int foo(T const(&)[N]) { return N; }
