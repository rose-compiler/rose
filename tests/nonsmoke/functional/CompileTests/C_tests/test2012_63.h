struct
   {
     int rla_flags;
  // This has to be in a struct (named or un-named) to demonstrate the bug.
     struct rlalink 
        {
           int x;
        } rla_link[1];
   } un_rla;

