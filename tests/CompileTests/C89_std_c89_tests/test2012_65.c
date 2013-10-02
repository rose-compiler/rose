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

