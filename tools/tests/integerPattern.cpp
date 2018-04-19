// array of integer types

void foo(int *ndx, int len, int * base, int jp)
{
  int *t1, *t2, *t3, *t4;

  t4= base;
  t1 = t4 + 1 ; 
  t2 = t1 + jp;
  t3 = t4 + jp;
  
  for (int i = 0; i < len; i++) {
    t1[ndx[i]] += 1;
    t2[ndx[i]] += 1;
    t3[ndx[i]] += 1;
    t4[ndx[i]] += 1;
  }
}
