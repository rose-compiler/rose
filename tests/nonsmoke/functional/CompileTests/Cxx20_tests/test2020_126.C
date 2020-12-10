

void foobar()
   {
     const int x = 5;
     int y = 5;
     if constexpr (x)
       {
         y = 42;
       }

   }
