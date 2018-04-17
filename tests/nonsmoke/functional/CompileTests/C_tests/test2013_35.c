// This example demonstates a goto from the true branch to the false branch...
// This is the essential subset of test2012_116.c

void foobar()
   {
     goto f_b;

     if (1)
        {
          41;
        }
       else
// f_b: {
        {
          if (2)
        f_b: {
          // {
             
               42;
             }
#if 1
            else
             {
               45;
             }
          43;
#endif
        }
   }
