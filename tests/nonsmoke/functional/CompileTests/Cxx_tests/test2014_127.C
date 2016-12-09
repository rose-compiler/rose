struct T // C++, C11
   {
     int m;
     union //anonymous
        {
          char * index;
          int key;
        };
   };

struct T t;

void foo()
   {
  // DQ (7/24/2014): This fails for EDG 4.9 (for C11)
     t.key = 1300; //access the union's member directly
   }

