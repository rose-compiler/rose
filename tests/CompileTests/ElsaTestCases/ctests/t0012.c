// t0012.c
// generalized ?: lvalues

void f(int x)
{
  int i,j,k;

  __checkType(x? i : j, k);       // lval
  __checkType(x? (i+1) : j, 1);   // rval
  __checkType(x? i : (j+1), 1);   // rval
}
