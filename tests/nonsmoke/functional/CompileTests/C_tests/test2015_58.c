struct Y
   {
     union 
        {
          struct { int x; } var_x;
          struct { float y; } var_y;
        } u;
   };

struct Y *X;

void foobar(void)
   {
     typeof(&X->u.var_x) __ptr;
   }

