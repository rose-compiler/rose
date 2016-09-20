// Test of designators in structs
struct T 
   {
     int w;
     char x;
   };

struct S 
   {
     int a[5];
     double b;
     struct T c;
   };

void foo() 
   {
     struct S x = {
       .b = 3.14,
#if 0
    // EDG doesn't produce the correct AST for these (not a ROSE issue)
       .c.w = 8,
       .c.x = 7,
#endif
    .c = {.x = 7, .w = 8},
#if 0
    a: { // An obsolete GCC syntax
      [1] = 1,
      [3] = 2,
      3
        },
#endif
        };
   }
