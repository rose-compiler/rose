// t0332.cc
// pass array where reference to const pointer is expected

void f(char * const &p);

void foo()
{
  char arr[2];
  
  f(arr);
  f("whatever");
}

