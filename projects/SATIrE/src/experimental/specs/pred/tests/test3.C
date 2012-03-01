void f(void)
{
  int x,y,z,s;
  s = 0;
  x = 1;
  y = x + 1;
  if(x<y)
    z=1;
  else
    x = 0;

  if(s>=0) {
    if(x<y) {
      while(z>0) {
        z=z-1;
      }
    }
  }
}
