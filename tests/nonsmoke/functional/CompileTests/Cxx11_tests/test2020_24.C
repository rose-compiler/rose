struct A 
   {
     int i;
   };

// Unparsed as: struct B  (missing base class ": A")
struct B : A 
   {
     int j;
   } d;

void foobar()
   {
     B x;
     x.i;
   }
