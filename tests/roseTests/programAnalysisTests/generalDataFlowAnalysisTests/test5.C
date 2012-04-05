int foo()
{
  int a, b, c;
  b = a + 2; //live_in={a},   OUT = {a,b}
  c = b * b; //     IN={b,a}, OUT = {a,c}
  b = c + 1;  //    IN={a,c}, OUT = {b,a}
  return b * a; //  IN={b,a}, OUT = {}

}


