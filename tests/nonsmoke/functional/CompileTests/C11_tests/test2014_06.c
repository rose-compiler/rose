// Test of C11 specific macros
#define cbrt(X) _Generic((X), long double: cbrtl, default: cbrt_other, float: cbrtf)(X)

void cbrtl(long double);
void cbrtf(float);
void cbrt_other(int);

void foobar()
   {
     long double a;
     float b;
     int c;

     cbrt(a);
     cbrt(b);
     cbrt(c);
   }
