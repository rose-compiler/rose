void f(void)
{
    int a, b=0, c=0;

    a = b + c;
    {
        int d = 7;
        {
            int e = d;
            e--;
        }
        d++;
    }
    if (a == b + c) {
      int x;
      x= 42;
      a=x;
    }
    else {
      int y;
      y = 23;
      a=y;
    }
}
