
void func()
{
  int x;
  int y;
  x = 0;
  y = 1;
  if (x == 0 && y == 1) {
    y = y + 1;
  }
  while(x < 10){
    x = x + 1;
    y = y + 1;
    y = y + 1;
    if (y == 4) {
      y = y + 1;
    }
  }
  int a[3];
  a[1] = 2;
  a[2] = a[1];
}

void __forward_func()
{
  int x;
  int y;
  rts.assign(&(x), 0);
  rts.assign(&(y), 1);
  if (x == 0 && y == 1) {
    rts.assign(&(y), y + 1);
  }
  while(x < 10){
    rts.assign(&(x), x + 1);
    rts.assign(&(y), y + 1);
    rts.assign(&(y), y + 1);
    if (y == 4) {
      rts.assign(&(y), y + 1);
    }
  }
  int a[3];
  rts.assign(&(a[1]), 2);
  rts.assign(&(a[2]), a[1]);
}
void __reverse_func() { rts.reverseEvent(); }
