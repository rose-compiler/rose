// The example extracted from GeorgeIterated1996, Figure 2.
// Liao 5/14/2012
void foo (int *mem, int j, int k, int bound)
{
  int g, h, f, e, m, b, c, d;

  g = mem [j +12];
  h = k -1;
  f = g*h;
  e = mem [j+8];
  m = mem [j+16];
  b = mem [f];
  c = e +8;
label1:  
  d =c;
  k = m +4;
  j = b;
  if (k < bound)
    goto label1;
}
