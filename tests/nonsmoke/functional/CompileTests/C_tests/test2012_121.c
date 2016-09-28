// This example demonstates a label on the true branch.
// This is the essential subset of test2012_116.c, and variations test2012_117-120.c

void foobar()
   {
  // The bug is that the lable is unparsed in the wrong place: "if {} f_b: else {}"
     if (1)
   f_b: {
          42;
        }
       else
        {
          43;
        }
   }

