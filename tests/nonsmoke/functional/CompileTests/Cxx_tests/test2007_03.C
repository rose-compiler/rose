
// If true then this will force a compile time error if the 
// bug happens (if the name qualification is dropped).
#define GENERATE_ERROR 1

class X
   {
#if GENERATE_ERROR
      static bool x;
#else
      static bool x_static_bool;
#endif
   };

#if GENERATE_ERROR
bool X::x             = false;
#else
bool X::x_static_bool = false;
#endif

int x = 0;

