
struct myClass
   {
     int operator-() { return 42; }
     int operator~() { return 42; }
   };

void foobar()
   {
     myClass x;
     unsigned int y;

     -x;
     ~x;
     ~y;
   }
