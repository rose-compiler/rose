void foobar()
   {
     unsigned int idx;

  // DQ (3/15/2015): The issue (bug) is the extra "{}" that is output before the for loop's body.
  // for ( idx = 0; idx < sizeof(struct { int; }); ++idx )
     for ( ; sizeof(struct { int x; }); )
        {
          break;
        }
   }
