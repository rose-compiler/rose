//Test case for insert before using comma operator
//Liao 3/14/2011

int & varFunc(int);
int fooA (int, int);
int fooB (int, int);
int bar1(int);
int bar2(int);
int bar3(int);
int bar4(int);

void foo(int a, int w, int x, int y, int z)
{
  varFunc(a) = fooA(bar1(w), bar2(x)) + fooB(bar3(y), bar4(z));
}


