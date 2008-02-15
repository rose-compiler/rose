// k0046b.cc
// test for ANSI mode

union U1 {
    struct {
        int foo1;
        int foo2;
    };
};

void f()
{
  U1 u;

  // the anon struct is useless, so this is illegal
  //ERROR(1): u.foo1;
}
