// THIS EXAMPLE MAY BE A REPEATED EXAMPLE THAT ALREADY EXISTS

// This is an example of a typedef that defines multiple type names,
// similar to how a variable declaration can define multiple variables.
// typedef declaration with multiple types declared
typedef struct __rose_generated_name_0 {
int a;}B;
typedef struct __rose_generated_name_0 *BP;

int foo(B b,BP bp)
{
 return (b.a) + (bp -> a);
}

// typedef declaration with multiple types declared
typedef int integer;
typedef int *integerPointer;
// variable declaration with multiple variables declared
struct __rose_generated_name_1 {
int x;
double y;}var1;
struct __rose_generated_name_1 *var2;

void foobar()
{
 var1.x = 42;
 var1.y = 3.14;
 var2 = &var1;
} 

