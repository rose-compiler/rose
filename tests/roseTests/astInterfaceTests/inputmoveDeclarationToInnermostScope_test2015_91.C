
#define MY_MACRO(a) ((a)+1);

void foobar()
   {
     int i;
     int my_var;
     int lenmap;

     lenmap = MY_MACRO(my_var);

     { i = 0; }
   }

