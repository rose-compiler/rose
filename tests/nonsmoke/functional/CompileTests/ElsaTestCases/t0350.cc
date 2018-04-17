// t0350.cc
// problem with argument deduction after a variable is bound

template <class T, class Y>
void f(int indent, T *t, Y (*map)(T const *t));

class Variable {};

int varName(Variable const *v);

void foo()
{
  Variable *v;
  f(3, v, &varName);
  f(3, v, varName);
}
