#define MY_MACRO(factor) { (factor) = 1; }

void foobar()
   {
     int a;
     MY_MACRO(a);
   }

