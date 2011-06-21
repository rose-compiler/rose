struct foo { const int bar = 7; };

template <int V>
class test {};

// Here is where name qualification is required for a SgVarRef (value).
typedef test<foo::bar> tt1;

// Here is where name qualification is required for a SgVarRef in an expression.
typedef test<foo::bar + 1> tt;
