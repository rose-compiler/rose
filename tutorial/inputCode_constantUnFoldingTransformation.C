int x;
void foo()
   {
  // Should constant fold to be "x = 100;"
     x = 21 + 72 + 7;
   }
