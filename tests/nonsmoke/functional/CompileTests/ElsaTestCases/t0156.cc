// t0156.cc
// a variant of d0026.cc

template <class T> struct F {};
template <class T> struct I {
  F<T> m;
};

// nasty: there is no fully-qualified name for 'char_type'!
//
// turn on the 'buildASTTypeId' tracing flag to see the solution work
void foo()
{
  typedef short* char_type;
  typedef F<char_type> f;
  typedef I<char_type> i;
}
