
#define MACRO_VALUE 12
#define MACRO_FUNCTION(x) ((x)+(x))

void foobar();

void foo()
   {
     int *i;

     int x = 2;
     x = MACRO_VALUE;
     x = MACRO_FUNCTION(42);

#if 0
  // ABC != XYZ
  /* ABC != XYZ 
   */
#if 0
#endif
     if (i != 0L)
     {
     }
#endif

  /* ABC != XYZ \
     aaa \
     bbb \
     ccc
   */
     
     foobar(i);
   }

