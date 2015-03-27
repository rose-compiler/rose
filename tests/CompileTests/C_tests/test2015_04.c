

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

struct XYZ
   {
     int a;
   };


void foobar()
   {
  // int abc[10];
     struct XYZ abc[10];

     if ( 5 < ARRAY_SIZE(abc))
        {
        }
   }

