#pragma fuse lc (oa, cp)

int * foo()
{
  int i = 1;
  int * p = &i;
  int A[100];
  A[1] = 99;

  *p = A[i];
  
  return p;
}
