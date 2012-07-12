
// Defining declaration of "struct b" (at this point we know the scope)
typedef struct b
   {
     int i;
   } BDEF;

void foo()
   {
     b x;
     x.i = 7;
   }
