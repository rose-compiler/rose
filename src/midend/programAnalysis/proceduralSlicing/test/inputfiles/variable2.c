/*
This is a file for testing re-definition and use of variables.
*/
int printf(const char *, ...);

int main()
{
  int x;
  x=2;
  x=x+4;
  x=4;
  x=x+3;
  x+=5;
  x;
  x+=7;
  
  int y;
  y=4+5;
  int z = x+y;

  return 0;
}
