
#include "stdio.h"

class boolean 
   {
     public:
          boolean (bool b) { printf ("constructor() \n"); }
          boolean operator! (void) { printf ("operator!() \n"); return true; }
          boolean operator= (boolean x) { printf ("operator=() \n"); return true; }
          boolean operator!=(boolean x) { printf ("operator!=() \n"); return true; }      
   };

int main()
   {
     boolean x = true;
     boolean y = true;
     printf ("Execute: x = ! y \n");
     x = ! y;
     printf ("Execute: x =! y \n");
     x =! y;
     printf ("Execute: x != y \n");
     x != y;
   }

