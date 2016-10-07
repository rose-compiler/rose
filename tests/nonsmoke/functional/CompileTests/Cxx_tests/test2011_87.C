struct foo { int bar; };

template <int foo::*V>
class test {};

typedef test<&foo::bar> tt;
