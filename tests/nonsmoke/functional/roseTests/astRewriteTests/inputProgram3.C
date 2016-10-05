// #include "stdio.h"

int globalVariableX;

#if 0
class A
   {
     public:
          A(): x(42) {};
          int x;
          int foo() { int y = foo(); return 0; }
   };
#endif

int main()
   {
     int x;
#if 0
     int y;
     int z;

     x + 1;

  // for (int i=0; i < 5; i++)
     int i;
     for (i=0; i < 5; i++)
        {
          x += i; 
          y += i; 
          z += i; 
        }
#endif

#if 0
     switch(x)
        {
          case 1: 
          {
          x++;
          break; 
          }
          
#if 1
          default:
             {
               x--; 
            // printf ("Error, default reached in switch! \n");
             }
#endif
        }
#endif

     return 0;
   }

