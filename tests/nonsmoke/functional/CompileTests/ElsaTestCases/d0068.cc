// testing two instantiations of class template with same integer
// argument in function template definition parameter mode (mode 2); I
// was attempting to test MM_ISO mode in matchtype.cc on int
// parameters instead of type parameters
template<int I> struct A;
template<int J> int g(A<J> a);
template<int J> int g(A<J> a) {
  return 1;
}
template<int I> struct A {
};
int main() {
  A<2> a2;
  g(a2);
}
