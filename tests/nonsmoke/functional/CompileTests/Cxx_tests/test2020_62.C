// This passes for g++ and clang++, but fails for gcc and clang (c mode).
// This also fails for EDG (c mode), and passes for EDG in C++ mode.
static void foobar(struct my_class* X);

struct my_class
   {
     int x;
   };

// This is reported as a conflicting function declaration.
static void foobar(struct my_class* X)
   {
   }
