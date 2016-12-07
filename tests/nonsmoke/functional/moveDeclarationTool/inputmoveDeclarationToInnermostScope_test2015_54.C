#define MY_MACRO(factor) if(1){ (factor) = 1; }

void foobar()
   {
     int a;
     MY_MACRO(a);
   }

