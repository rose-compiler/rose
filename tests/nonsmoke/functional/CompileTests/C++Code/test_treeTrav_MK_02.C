int foo(int x)
{
  return x*x;
}


int main(void)
{
  int a, b;
  a= foo(1);
  b= foo(2);
  return a+b;
}
