class A
{
};

class B
{
};

int foo( A *a, B *b )
{
  return 0;
}

int main()
{
  A *a = new A;
  B *b = new B;
  int i = foo( a, b ); //ok...

  return foo( new A, new B ); //bad
}
