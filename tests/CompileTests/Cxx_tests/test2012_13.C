// Note that if we set #define SKIP_ROSE_BUILTIN_DECLARATIONS
// in the rose_edg_required_macros_and_function.h file then 
// this causes an error for this code.  I don't know why!
// This value is only set for debugging, and so it is not
// a problem in reality, but it is curious and should be
// looked into at some point.

class XXX
   {
     public:
         typedef XXX *XXXstar;

         XXX (int) {}
   };

