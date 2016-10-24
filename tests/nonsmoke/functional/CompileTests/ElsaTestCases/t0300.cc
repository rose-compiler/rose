// t0300.cc
// char** to void*

void f(void *);

void foo()
{
  char **p;
  f(p);

  //ERROR(1): char const *cp;
  //ERROR(1): f(cp);
  
  // according to gcc this is ok..
  char const **cpp;
  f(cpp);
}
