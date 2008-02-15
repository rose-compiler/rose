// t0358.cc
// isolated from nsHTMLEditRules, this syntax is causing a segfault


typedef int Int32;

struct Foo {
  int Length();
};


void foo(Foo *f)
{
  //(Int32)(*f).Length();

  3 < (Int32)(*f).Length();
}
