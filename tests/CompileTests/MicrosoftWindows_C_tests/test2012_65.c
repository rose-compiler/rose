int x;

#if 1
/* Review this issues later (problem with MSVC support for un-named unions, now fixed). */
struct X
  {
    union
    {
      int x;
    } v;
  };

struct Y
  {
    union
    {
      int x;
    } *v;
  };

#if 1
void foobar()
   {
     struct X A;
     struct Y B;
     int x;
     x = A.v.x;
     x = B.v->x;
   }
#endif
#endif

