class A
   {
     public:
          static const int n = 12;
          int x:2, y:6;
          int z:n;        // Error: Unparsed code does constant substitution

          void foo()
             {
               x = 7;  // 7 should require 3 bits
               y = 2;
               z = 7;
             }
   };
