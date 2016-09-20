// t0011.c
// implicit function declaration

//ERROR(1): int g(int,int);

void f()
{
  g(3);
}

void f2()
{
  g(3,4,5,6);
}

int g();

int g(int x)
{
  return x;
}

int g(int);


//ERROR(2): int h(int x, ...);

int h();

int h(int, int);
