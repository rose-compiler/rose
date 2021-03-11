
struct ABC
   {
     int x;
   };

void foobar (struct ABC abc)
   {
   }

void wrapper_foobar_1 (struct ABC abc)
   {
     foobar(abc);
   }

void wrapper_foobar_2 (struct ABC abc)
   {
     struct ABC xyz;
     foobar(xyz);
   }

