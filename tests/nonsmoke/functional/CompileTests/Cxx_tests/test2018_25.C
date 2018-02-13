namespace X
   {
     int k = 21;
     int f() { return k + 73; }
   }

namespace Y
   {
     namespace Z = X;
   }

int iequals(int, int);

void foobar()
   {
  // identifier in namespace-alias-definition is synonym for name of
  // namespace denoted by qualified-namespace-spec and becomes
  // namespace-alias
     iequals(::Y::Z::k, 21);
     namespace A = ::Y::Z;
     iequals(A::f(), 94);
   }

