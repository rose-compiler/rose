#define MY_MACRO(factor) { (factor) = 1.0; }

// The macro is recorded as part of the function's token stream (not as white-space).
// It appears that it is because it is a non-defining function (works when it is a defining function).
double foo(double);

void foobar() 
   {
     int a;

     MY_MACRO(a);

   }

