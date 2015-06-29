struct Y
   {
     int abc;
   };

struct Y *X;

void foobar(void)
   {
     typeof(&X->abc) __ptr;
   }

