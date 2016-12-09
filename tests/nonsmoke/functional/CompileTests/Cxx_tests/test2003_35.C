// #include "stdio.h"
#define NEW_BLOCK 0

int main()
   {
     int x = 1;
     switch(x)
        {
          case 1:
             { 
#if NEW_BLOCK
             { 
#endif
               int y = 47;
               y++; 
#if NEW_BLOCK
             }
#endif
             }
        }

     return 0;
   }
