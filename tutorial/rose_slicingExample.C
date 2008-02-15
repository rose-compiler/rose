/* 
  Example for the tutorial
*/
extern int add(int a,int b);

int main()
{
  int x;
  int y = (5);
  int a = (8);
  int b = (7);
  x = 4;
  b = 2;
  int i = (add(x,y));
  if (i <= y + 40 && y != a) {
    x = x * i;
    a = a + x;
    b = a;
  }
  else {
  }
  y = x + b;
  return 0;
}


int add(int a,int b)
{
  int c;
  c = a + b;
  return c;
}

