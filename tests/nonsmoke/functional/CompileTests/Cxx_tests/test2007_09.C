
// If true then this will force a compile time error if the 
// bug happens (if the name qualification is dropped).
#define GENERATE_ERROR 1

class Y
   {
     public:
          int x;
   };

class X
   {
     public:
          static Y* x;
   };

Y* X::x = 0L;
int x = 0;

void foo()
   {
     X* x;

  // Access through data member (to variable which happens to also be static) (unparses to: "( *(x -> x)).x = (false);")
     x->x->x = false;

  // Access through static variable (error: unparses to "x -> x = (true);"
     X::x->x = true;
   }
