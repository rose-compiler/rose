namespace CHSystem
   {
     class X
        {
          int memalign();
        };

// This function will not be unparsed!
// Actually it appears to not be in the AST!!!
     int X::memalign()
        {
          return 0;
        }
   }
