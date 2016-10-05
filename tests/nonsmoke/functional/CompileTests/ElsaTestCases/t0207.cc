// t0207.cc
// default template type arguments

template <class S, class T = int>
class C {};

C<float> c;
