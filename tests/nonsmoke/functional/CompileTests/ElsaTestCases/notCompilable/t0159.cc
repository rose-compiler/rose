// t0159.cc
// namespace alias

asm("collectLookupResults x=7");

namespace N1 {
  int x;                 // line 7
}

namespace N2 = N1;

int f()
{
  return N2::x;
}


// see what happens if I use a template name
template <class T>
struct S {
  struct Q;
};

//ERROR(1): namespace N3 = S<int>;
//ERROR(2): namespace N4 = S<int>::Q;
