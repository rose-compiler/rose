
int main()
{
  int a;
  int b;
  int c;
  a = 3;
  b = a;
  while(a < 10){
    if (a < b) {
      a = (a + 1);
    }
    else {
      b = (b + 1);
    }
    c = (a + b);
  }
  a = c;
  return 0;
}

