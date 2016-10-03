class ABC<int>
   {
     public:
          int xyz;
          int foo();
   };

int ABC<int>::foo ()
   {
     return xyz;
   }


