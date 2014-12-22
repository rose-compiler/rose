
struct X;

struct Y
   {
     struct X *a;
   };

#if 0
  /* Not allowed in Microsoft MSVC */
struct X
   {
   };
#endif
