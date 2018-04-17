// t0326.cc
// invoke a function through a function pointer

void (*func)(int);

void f()
{
  func(3);
  //ERROR(1): func();     // wrong # of args
}
