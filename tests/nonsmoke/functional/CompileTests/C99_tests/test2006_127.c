// this test code demonstates a problem with conditionals that don't 
// contain an explicit basic block (SgBasicBlock).

void foobar()
   {
     int x = 1;
     int y = 1;
     if ( x != y ) // this works
        {
          x = +x;
        }

     if ( x != y ) // this can fail
          x = +x;
   }

