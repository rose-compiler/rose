namespace N
   {
     class color;
   }

typedef class N::color {} new_type;

void foobar()
   {
     new_type x = N::color();
   }
