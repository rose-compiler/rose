/*
 * Combined parallel for
 * with multiple clauses
 * */
int main(void)
{
  int i, a[1000];
  int sum;
#pragma omp parallel for if(1) ordered reduction(+:sum) schedule(dynamic, 5) 
  for (i=0;i<1000;i++)
  {
    a[i]=i*2;
    sum+=i;
  }
  return 0;
}
