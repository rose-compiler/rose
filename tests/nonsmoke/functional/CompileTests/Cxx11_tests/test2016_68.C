struct T3
   {
     int mem1;
     T3() { } // user-provided default constructor
   };

struct T4
   {
     T4(T3 x); 
   };

int main()
   {
     T4 x({});
   }
