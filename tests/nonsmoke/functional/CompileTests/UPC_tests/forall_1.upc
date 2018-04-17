#include <stdio.h>
int main()
{
  int i;
  {
  upc_forall(
    int j=0;
    j<10;
    j++;
    j)
   printf("j=%d\n",j);
  }
  {
   for(int j = 0; j < 10; ++j) {}
  }
  {
 upc_forall(i=0;i<10;i++;continue)
   printf("i=%d\n",i);
  }

{
 for(i=0;i<10;i++)
   printf("i=%d\n",i);
}
  return 0;
}
