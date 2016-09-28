// t0567.cc
// throw-spec irrelevant in type comparison for ?:

void foo(int c)
{
  int (*f1)(char const *, char const *);
  int (*f2)(char const *, char const *) throw();

  c? f1 : f2;
}
