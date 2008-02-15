// 7.3.4b.cc

namespace M {
  int i;
}

namespace N {
  int i;
  using namespace M;
}

void f()
{
  using namespace N;
  //ERROR(1):   i = 7;           // error: both M::i and N::i are visible
}
