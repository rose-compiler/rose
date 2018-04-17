
void foobar()
   {
     int x;
//   if(1) if(2) if(3) if(4)
     x = ( ({ union { int i; } u; u.i = 42; }) == 0) ? ({ union { int i; } u; u.i = 42; }) : ({ union { int i; } u; u.i = 42; });

     42;
     43;
     44;
     45;
     46;

     if ( ({ union { int i; } u; u.i = 47; }) == 0)
        {
          int x = ({ union { int i; } u; u.i = 47; });
        }

     x = 47;

     if ( ({ union { int i; } u; u.i = 48; }) == 0)
        {
          int x = ({ union { int i; } u; u.i = 48; });
        }

     x = 48;

     if ( ({ union { int i; } u; u.i = 49; }) == 0)
        {
          int x = ({ union { int i; } u; u.i = 49; });
        }

     x = 49;

     if ( ({ union { int i; } u; u.i = 42; }) == 0)
        {
          int x = ({ union { int i; } u; u.i = 42; });
        }

     x = 50;

     if ( ({ union { int i; } u; u.i = 42; }) == 0)
        {
          int x = ({ union { int i; } u; u.i = 42; });
        }

     x = 51;

     if ( ({ union { int i; } u; u.i = 42; }) == 0)
        {
          int x = ({ union { int i; } u; u.i = 42; });
        }

     x = 52;

     if ( ({ union { int i; } u; u.i = 42; }) == 0)
        {
          int x = ({ union { int i; } u; u.i = 42; });
        }

     x = 53;

     if ( ({ union { int i; } u; u.i = 42; }) == 0)
        {
          int x = ({ union { int i; } u; u.i = 42; });
        }

     x = 54;

   }
