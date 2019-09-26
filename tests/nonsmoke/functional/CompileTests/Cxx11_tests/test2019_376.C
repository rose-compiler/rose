
struct X
   {
     struct Y
        {
          int i, j;
        };
   };


void foobar()
   {
  // Unparses as: int    Y::* *p1;
  // Should be:   int X::Y::* *p1;
     int X::Y::* *p1;
   }
 
