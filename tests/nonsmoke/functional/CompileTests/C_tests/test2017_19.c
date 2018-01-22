#include "stdio.h"

void foo(int x)
   {
     int y = -1;
     switch (x)
        {
          case 0:
               if (x >= 0)
                  {
                    y = 42;
                    break;
                  }
                 else
          case 1: {
                    y = 43;
                  }
               break;

          default:
             {}
        }

     printf ("y = %d \n",y);
   }

int main()
   {
     foo(0);
     foo(1);
     foo(2);
   }
