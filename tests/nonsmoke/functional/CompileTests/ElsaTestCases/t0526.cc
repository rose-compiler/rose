// t0526.cc
// use __PRETTY_FUNCTION__ as argument to overloaded function

void f(void const *p);
void f(bool n);

void foo()
{
  // this is the type I give __PRETTY_FUNCTION__, and I am
  // seeing the same problem with this one
  static char const pfunc[] = "blah";
  f(pfunc);
  
  // apparently supposed to work same way as pointers
  char const *ptr = 0;
  f(ptr);

  f(__PRETTY_FUNCTION__);
}
