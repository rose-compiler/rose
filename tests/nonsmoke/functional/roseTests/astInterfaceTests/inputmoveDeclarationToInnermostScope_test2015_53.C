#define MY_MACRO(factor) { (factor) = 1.0; }

// The leading whitespace is correctly computed here
// (where as in test2015_52.C is it not correctly computed).

int x;

void foobar() 
   {
     int a;

     MY_MACRO(a);

   }

