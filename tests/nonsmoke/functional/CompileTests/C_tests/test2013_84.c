// Test code using token pasting.

#define TOKY(x,y) x##y;                         \
   

void foobar()
   {
     int a;
     int TOKY(a,b)
     int c;
   }

