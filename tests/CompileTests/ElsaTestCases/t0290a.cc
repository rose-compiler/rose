// t0290a.cc
// essence of the difficulty with t0290.cc

template <class T>
struct B {
  typedef int INT;
  INT foo(int);
};

// the type is called "B<T>::INT" (a DependentQType) out here, but is
// called simply "INT" (which maps to "int", a SimpleType) in the
// scope of B (above); to match this defn with the decl, we have to
// re-process the type once the declarator scope (B<T>) has been
// pushed
template <class T>
typename B<T>::INT B<T>::foo(int)
{
  return 2;
}

int f()
{
  B<int> a;
  return a.foo(1);
}


// EOF
