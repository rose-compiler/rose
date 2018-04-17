// This example demonstates a goto from the true branch to the false branch...
// This is the essential subset of test2012_116.c

void foobar()
   {
     goto f_b;
     if (3)
       f_b: 1;
   }
