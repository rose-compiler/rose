
#define MACRO(x) ((x)+(x))

void foobar();void foo()
   {
     int *i;
     int x = MACRO(42);
     foobar(i);
   }
