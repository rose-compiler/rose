#define MY_MACRO(x,y) x = 10; y = 11;

void foobar()
   {
     int i;
     int x1,y1;

     { i = 42; }

#if 0
     int ABC;
#endif

     MY_MACRO(x1,y1);
   }

