/*
 * An anti-dependence example
 */
void foo()
{
  int i;
  int a[100];

  for (i=0;i<99;i++)
    a[i]=a[i+1]+1;
}  
