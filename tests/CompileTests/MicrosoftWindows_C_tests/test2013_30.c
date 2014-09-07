
#if 0
/* No support for transparent unions declarations (GNU extension) (or maybe also un-named unions) under Windows */
union X
   {
      char* b;
      int a;
   } __attribute__ ((__transparent_union__));

typedef union
   {
      char* b;
      int a;
   } Y __attribute__ ((__transparent_union__));

void foo1 (union X x);
void foo2 (Y y);

void foobar()
   {
     union X x = {.a = 1};
     foo1(x);

     Y y = { .a = 2 };
     foo2(y);
   }
#endif

void abc();


   
