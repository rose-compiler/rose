#include <stdio.h>
int main()
{
  int i;
  upc_forall(i=0;i<10;i++;continue)
   printf("i=%d\n",i);
  return 0;
}
