int a;

class B
{
  int b;
  void foo(int c)
  {
    int d;
#pragma rose_outline
    b = a+c +d;
  }

};
