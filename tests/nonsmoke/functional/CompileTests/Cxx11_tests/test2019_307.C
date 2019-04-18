class X_
   {
          int i;
     public:
          X_(int ii);
          operator int();

       // Original code: explicit operator long();
       // Unparsed as:   operator long();
          explicit operator long();
   };

class Y_
   {
          double d;
     public:
          Y_(double dd);
          operator double();

       // Original code: explicit operator float();
       // Unparsed as:   operator float();
          explicit operator float();
   };

void foobar()
   {
     X_ x = 1;
     Y_ y = -1.0;
     x > 0 ? x + 1 : int(x);
     y > 0 ? double(y) : y - 1;
     int i = x;

     while (x)
        {
          x == i--;
          x = x - 1;
        }

     x;

     if (y)
          y = y + 1.0;
       else
          y = y - 1.0;

     y == 0.0;
   }

