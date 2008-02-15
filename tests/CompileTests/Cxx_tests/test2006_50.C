
// The following code demonstrates a semantic different
// between the use of "{}" and not using them.
// When DEMO_BUG is set to 1 the result is 0
//      DEMO_BUG is set to 0 the result is 1
// value checked using "echo $status"


#define DEMO_BUG 1

int foo(int x)
   {
     int y = 0;
     switch (x)
        {
          case 0:
#if DEMO_BUG
             {
#endif
               int y;
               y = 1;
#if DEMO_BUG
             }
#endif
          case 1: x = y;
        }

     return x;
   }

int main ()
   {
     return foo(0);
   }
