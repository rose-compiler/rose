
#define GET_NAME(x) x->name;
#define NULL 0L

struct foo_t
   {
     int name;
   };

void foobar()
{
   foo_t* srclist;

   int i ;
   while (srclist != NULL) {

      int         name     = GET_NAME(srclist)
      int j;

      i = 42;

   }
}
