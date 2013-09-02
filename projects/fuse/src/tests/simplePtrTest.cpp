#pragma fuse lc(ccs, pt, cp)

void CompDebugAssert(bool);

int main()
{
  int *p, val;
  int **q;
  val = 5;
  p = &val;
  q = &p;
  CompDebugAssert(*p + **q == 10);
  return *p + **q;
}
 
