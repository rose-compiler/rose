
enum A
   {
     ENUM_A
   };

void foobar()
   {
     A x;
#if 1
     switch(x)
       {
         case ENUM_A:
           {}
       }
#endif
   }
