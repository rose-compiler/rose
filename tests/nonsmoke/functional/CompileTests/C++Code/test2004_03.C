// This test code has several instances of the same bug in the EDG/SAGE connection.
// The problem is fixed (1/14/2004), the current source seq poitner is no longer 
// conditionally incremented (which allows it to be NULL in some cases).

#if 1
// this works fine
struct foo_A
   {
     int x;
   } varA1;

typedef struct foo_A varA2;
#endif

#if 1
// this works fine
struct foo_B{ int x; };
typedef struct foo_B varB;
#endif

#if 1
int
function_A () {
  typedef int myint;

  return 0;
}
#endif


#if 1
// Causes a assertion failure in EDG/SAGE connection
struct foo_C
   {
     int x;
   } varC1;

int function_B (int argc, char* argv[])
   {
     typedef struct foo_C varC2;
     return 0;
   }
#endif


