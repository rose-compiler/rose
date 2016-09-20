/* Test new line in pragma 
 *
 *  6/15/2010
 * Liao
 * */


void foo()
{
  int i, sum[100];

  for (i =0; i<100; i++)
  {
#pragma rose \
    outline 
    {
      sum[i] = i;
    }

  }
}
