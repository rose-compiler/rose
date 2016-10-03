// dC0018.c
// this testcase provokes a const-eval of a still-ambiguous
// expression, due to errors above the ambig expr

void aa_fastrender (int x1, int y1, int x2, int y2)
{
  if (x2 < 0 || y2 < 0 || x1 > (3) || y1 > (4))
    return;
}
