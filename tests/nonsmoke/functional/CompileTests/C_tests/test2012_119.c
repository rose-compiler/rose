// This example demonstates a goto from the true branch to the false branch...
// This is the essential subset of test2012_116.c

void foobar()
   {
     if (1)
        {
          if (2)
            {
              if (3)
                   44;
              41;
              goto f_b;
            }
        }
       else
//      {
          if (2)
        f_b: {
               42;
             }
            else
             {
               45;
             }
     43;
//      }
   }
