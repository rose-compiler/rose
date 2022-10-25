
int f(int x)
{
  x = x + 1;
  return x;
}

int main()
{
  int a;
  int b;
  a = 1;
  int __temp0__ = f(a);
  f(__temp0__);
  int __temp1__ = f(a);
  b = f(__temp1__);
  return 0;
}
