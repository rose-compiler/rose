// error: no template parameter list supplied for `A'

// note that the 'static' is critical for the bug

// sm: This is actually invalid C++, because there should be
//   template <>
// before the definition of A<int>::s.  However, gcc-2 accepts
// it (a bug), as does icc (apparently for compatibility with
// the gcc bug), though gcc-3 does not.  So, Elsa will probably
// have to support this too, if icc found it necessary.

// 2005-03-05: screw it; if gcc-3 does not support it, elsa
// does not have to either


template<class T> class A {
  static int s;
};

template<>
int A<int>::s = 0;
