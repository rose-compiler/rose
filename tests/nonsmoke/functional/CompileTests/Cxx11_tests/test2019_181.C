class X
   {
     public:
          int operator <<= (int b);
          int operator >>= (int b);
   };

void foobar()
   {
     X x;
     x <<= 4;
     x >>= 5;
   }
