// struct b;

struct a
   {
  // Need to get the scope of the non-defining declaration to "b" set properly to be SgGlobal.
     struct b *bp;
   };

#if 0
// Defining declaration of "struct b" (at this point we know the scope)
typedef struct b
   {
     int i;
   } BDEF;
#else
struct b;
struct b;
int abcdefg;

struct b {};

#endif
