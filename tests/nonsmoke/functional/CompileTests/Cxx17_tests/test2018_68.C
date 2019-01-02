// inheriting constructors

struct A {
  template<typename T> A(T, typename T::type = 0);
  A(int);
};
struct B : A {
  using A::A;
  B(int);
};
B b(42L); // now calls B(int), used to call B<long>(long),
          // which called A(int) due to substitution failure
          // in A<long>(long).


