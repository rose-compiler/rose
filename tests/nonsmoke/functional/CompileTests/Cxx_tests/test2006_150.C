// This is an example of a typedef that defines multiple type names,
// similar to how a variable declaration can define multiple variables.

// typedef declaration with multiple types declared
typedef struct {
   int a;
} B, *BP;

int foo(B b, BP bp) {
   return b.a + bp->a;
}

// typedef declaration with multiple types declared
typedef int integer, *integerPointer;

// variable declaration with multiple variables declared
struct 
   {
     int x;
     double y;
   } var1, *var2;

void foobar()
   {
     var1.x = 42;
     var1.y = 3.14;
     var2 = &var1;
   }

enum 
   {
     ZERO, ONE, TWO 
   } numbers, more_numbers;


