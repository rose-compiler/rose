#if 1
   #include "test2012_63.h"
#else

// Un-named struct containing named nested struct.
struct
   {
     int rla_flags;
  // This has to be in a struct (named or un-named) to demonstrate the bug.
     struct rlalink 
        {
          int x;
        } rla_link[1];
   } un_rla;
#endif

void foobar()
   {
  // Unparses incorrectly as: "register const struct rlalink { int x; } *rlp;" if the defining declaration is in the same file, and
  // unparses incorrectly as: "register const struct rlalink {} *rlp;" if the defining declaration is in a different (header) file.
     register const struct rlalink *rlp;

     int a;
     a = rlp->x;
   }
