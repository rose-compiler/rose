// 7.3.3d.cc

struct X {
  int i;
  static int s;
};

void f()
{
  //ERROR(1): using X::i;       // error: X::i is a class member
                                // and this is not a member declaration.
  //ERROR(2): using X::s;       // error: X::s is a class member
                                // and this is not a member declaration.
}
