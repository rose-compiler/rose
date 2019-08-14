#include <stdio.h>
 

int main() {

  int val = 0;

  int val2 = -1;

  scanf("%d", &val);

 

  switch(val)

  {

   case 0:

     val2 = 0;

     break;

   case 1:

     val2 = 1;

     break;

   default:

     val2 = 100;

     break;

   }

return val;

}
