// Declaring non-type template parameters with auto

template <int I, int J> struct A {};
template <int I> struct A<I, I> {};        // OK

template <int I, int J, int K> struct B {};
template <int I> struct B<I, I*2, 2> {};   // OK

