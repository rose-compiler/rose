// #include "stdio.h"
#define NEW_BLOCK 1

int main()
   {
     int x = 1;
     int z = 1;
     switch(x)
        {
          case 1:
#if NEW_BLOCK
             { 
#endif
               int y = 47;
               y++; 
#if NEW_BLOCK
             }
#endif
        }

     return 0;
   }

