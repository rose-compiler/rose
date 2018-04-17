// Found a bug that we were attempting to assign const members in the
// body of an implicitly generated copy assign operator.
struct A {};
struct B {
  const A x;
};
