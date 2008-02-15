// t0200.cc
// template type params with defaults

template <class T = int>
class C {};

// should use the default argument
C<> x;
