
struct A 
   {
   };

// BUG: unparsed as: struct B {}d;
struct B : A 
   {
   } d;
