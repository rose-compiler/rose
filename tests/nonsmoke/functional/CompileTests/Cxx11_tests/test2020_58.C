
typedef unsigned long size_t;

void foobar()
   {
     struct X;
     static X *px;
     struct X
        {
          void *operator new(size_t n, float f)
             {
               return 0L;
             }

          void *operator new[](size_t n, float f, char *p)
             {
               float res = 42.0;
               return 0L;
             }
        };

     X ax[10];
     px = ax;
     X *qx = new (4.2) X;
     static char buf[] = "[]";
     qx = new (4.3, buf) X [10];
   }


