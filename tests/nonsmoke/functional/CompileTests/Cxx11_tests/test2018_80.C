void foobar( const int* x );

namespace XXX {
typedef const int const_int;
}

// Need name qualification for the type.
void foobar( XXX::const_int* x );

