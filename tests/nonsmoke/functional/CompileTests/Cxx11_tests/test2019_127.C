namespace N
   {
     struct color;
   }

typedef struct N::color { static int x; } new_type;

void foobar()
   {
     N::color new_color;

     new_type y = new_color;

     int z = N::color::x;

  // This is not meaningful.
  // int aa = new_color.x;
   }
