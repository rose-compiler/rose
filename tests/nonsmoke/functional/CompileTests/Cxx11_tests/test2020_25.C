struct A 
   {
     int i;
   };

// Unparsed as: struct B  (missing base class ": A"
typedef struct B : A 
   {
     int j;
   } d_type;

void foobar()
   {
     d_type x;
     x.i;
   }
