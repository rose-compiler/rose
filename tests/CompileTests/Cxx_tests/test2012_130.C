class X
   {
     public:
#if 1
          int getValue() const
             {
               return arraySize;
             }
#endif
          static int arraySize;
   };

int X::arraySize = 16;
