
#define foo(x) \
   { x; }


void foobar()
   {
     int test;

     foo(4)
     foo(10)

     if (1)
        {
          test = 1;
        }
   }
