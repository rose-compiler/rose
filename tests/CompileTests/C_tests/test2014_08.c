// Testing the new macro handling support in ROSE.

// declaration of macro
#define MACRO_42 42

struct 
   {
     int value;
   } X;

#define value X.value;


void foobar()
   {
  // Use of macro.
     int x = MACRO_42;
     int y = value;
   }


