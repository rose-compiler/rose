class X
   {
   };


X foo()
   {
     X object1;

  // Should be unparsed as: "X object2 = X();"  but is equivalent as "X object2();"
     X object2 = X();

     X object3 = object1;
   }

