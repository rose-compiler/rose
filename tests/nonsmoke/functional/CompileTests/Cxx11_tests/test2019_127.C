namespace N
   {
     struct color;
   }

// Typedef base type struct should reference the one in the namespace N.
// Should be unparsed as: "N::color" instead of "color".
typedef struct N::color { static int x; } new_type;

void foobar()
   {
     N::color new_color;

     new_type y = new_color;

     int z = N::color::x;

  // This is not meaningful.
  // int aa = new_color.x;
   }
