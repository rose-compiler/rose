struct point { int x, y; };

void foobar()
   {
  // These must be constants in there were in global scope (see test2016_18.c).
     const int xvalue = 1;
     const int yvalue = 1;

     struct point p = { .y = yvalue, .x = xvalue };
   }
