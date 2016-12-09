// g0009.c
// test __FUNCTION__ etc. in C mode

void f()
{
  char *a = __func__;
  char *b = __FUNCTION__;
  char *c = __PRETTY_FUNCTION__;

  //ERROR(1):   char *d = something_else;             // undefined

  //ERROR(2):   char *d = "x" __func__;               // no concat

  // these *can* concat in gcc <= 3.3
  char *f = "x" __FUNCTION__;
  char *g = "x" __PRETTY_FUNCTION__;

  char *f2 = "x" __FUNCTION__ "y";
  char *g2 = "x" __PRETTY_FUNCTION__ "y";
}
