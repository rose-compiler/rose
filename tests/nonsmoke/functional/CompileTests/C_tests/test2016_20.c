struct point { int x, y; };

void foobar()
   {
  // These must be constants if they were in global scope (see test2016_18.c).
     int xvalue = 1;
     int yvalue = 1;

     struct point p = { .y = yvalue, .x = xvalue };
   }
