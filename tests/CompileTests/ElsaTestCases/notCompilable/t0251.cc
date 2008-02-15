// t0251.cc
// simple example with anonymous namespaces

namespace {
  int a;
  int b;
}

namespace {
  int b;
  int c;
}

void f()
{
  a;        // from first one
  //ERROR(1): b;        // ambiguous
  c;        // from second one
}
