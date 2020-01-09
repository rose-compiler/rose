class X
   {
     public:
          int operator , (int x);
          X(int x);
   };

enum my_enum { enum0 };

// int operator , (X  s, my_enum j);

void foobar()
   {
     X x (1);

  // Unparsed as: int a = x , 1;  (without parenthesis)
     int a = (x , 1);

#if 0
     int b = 0;
     b = (x , 1);

     int y = (4,5);
#endif
   }

