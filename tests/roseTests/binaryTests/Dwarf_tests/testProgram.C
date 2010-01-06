// #include<stdio.h>

// int this_is_a_variable = 0xDeadBeef;
int this_is_a_variable = 0x01234567;

#define FOOBAR_NEGATE_MACRO(x) -(x)

static double global_static_x_var = 0;

static void foobar_static_function() {}

typedef int foo_int;

class foo_class
   {
     public:
          int x;
   } X_class;



int
main() {
     int another_variable = this_is_a_variable + 7;

     static float local_static_x_var = 0;

#if 1
     int x = 7;
     int y,z;
     y = x;
     z = y;

  // Test use of macro in Dwarf
     x = FOOBAR_NEGATE_MACRO(y);
     z = FOOBAR_NEGATE_MACRO(y);
#endif

     char* myString = "ABCDEFG";

  // printf ("Exiting");

     return another_variable;
   }
