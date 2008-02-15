// testing two instantiations of class template with same integer
// arguments
template<int I> struct A {
};
A<2> a2;
A<2> a2a;
A<3> a3;
