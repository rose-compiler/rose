// some testing of int template parameters
template<int I> struct A;
template<int I> int g(A<I> a);
template<int I> int g(A<I> a);
template<int I> int g(A<I> a) {
}

template<int I> struct A {
};

A<2> a2;
A<3> a3;

int main() {
  g(a2);
  g(a3);
}
