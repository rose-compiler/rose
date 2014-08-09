#if 0
#endif

void func()
{
  int *a = new int ();
  int x;
  int y;
  int z;
  x = 1;
  z = 2;
  if (x == 1) {
    x = 1;
  }
  if (x == 1) {
    x = 2;
  }
  else {
    x = 3;
    y = 4;
  }
   *a = 5;
  y = x;
  z = 1;
  if (x > 1) {
    z = z + x + y;
  }
   *a = z;
  operator delete(a);
}
#if 0
#endif

void __forward_func()
{
  int *a = new int ();
  int x;
  int y;
  int z;
  rts.assign(&(x), 1);
  rts.assign(&(z), 2);
  if (x == 1) {
    rts.assign(&(x), 1);
  }
  if (x == 1) {
    rts.assign(&(x), 2);
  }
  else {
    rts.assign(&(x), 3);
    rts.assign(&(y), 4);
  }
  rts.assign(&(*a), 5);
  rts.assign(&(y), x);
  rts.assign(&(z), 1);
  if (x > 1) {
    rts.assign(&(z), z + x + y);
  }
  rts.assign(&(*a), z);
  operator delete(a);
}
#if 0
#endif
void __reverse_func() { rts.reverseEvent(); }
