int globalVar;

void f(void)
{
  int x;
  x=1;
  globalVar=2;
  globalVar=globalVar+x;
  x=globalVar;
}
