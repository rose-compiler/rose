
// Input for translator to show exception-based exiting from a translator.

namespace A
{
  int __go__;
  struct B
  {
    static int __stop__;
  };
};

void foo (void)
{
  extern void bar (int);
  bar (A::__go__);
  bar (A::B::__stop__);
}
