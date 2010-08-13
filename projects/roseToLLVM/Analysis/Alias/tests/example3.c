#include <stdio.h>

int main()
{
   // Must alias
   int *p, *q;
   int i;
   i = 10;
   p = &i;
   //q = p;
   (*p)++;
   //*q = *p + 5;
   printf("%d\n", i);

   return 0;
}
