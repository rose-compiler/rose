void f(void)
{
  int x,y,z,s;
  s = 0;
  x = 1;
  y = x + 1;
  if(x<y)
    z=1;
  else
    z=-1;

  if(s>=0) {
    while(x<z*y) {
      if(s<100) {
	x=x+1;
	s=s+z;
      }
      y=y-1;
      while(x<2) {
        x=x+1;
      }
    }
  }
}
