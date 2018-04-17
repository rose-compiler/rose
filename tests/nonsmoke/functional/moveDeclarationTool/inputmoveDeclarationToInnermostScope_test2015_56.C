#define MY_MACRO(factor) { int (factor) = 1; }

void foobar()
   {
     int a;
     MY_MACRO(a);
   }

