int
f1 ( enum {FALSE, TRUE} b )
   {
     return FALSE;
   }

enum {FALSE, TRUE}
f2 ( enum {FALSE, TRUE} b )
   {
     return FALSE;
   }

enum boolean1 {FALSE1, TRUE1}
f3 ( enum {FALSE1, TRUE1} b )
   {
     return FALSE1;
   }

enum boolean2 {FALSE2, TRUE2}
f4 ( enum boolean3 {FALSE3, TRUE3} b )
   {
     return FALSE3;
   }

#if 0
// This is an error!
enum boolean3 {FALSE_fail, TRUE_fail}
f5 ( boolean3 b )
   {
     return FALSE_fail;
   }
#endif

void foo8 ( struct { int x; } s )
   {
   }

#if 0
struct { int x; } foo6 ( );
struct { int x; } foo7 ( struct { int x; } s );
struct Atag { int x; } foo8 ( struct { int x; } s )
   {
  // return struct { int x; } A;
  // struct { int x; } A;
     Atag* A;
     return *A;
   }
#endif



