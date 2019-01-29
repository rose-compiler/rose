struct A
   {
     int b { 42+42*3 };
     int x; 
     explicit constexpr A(int e) : x(e) { }
     constexpr operator int() { return x+b; }
   };


void foobar()
   {
     A a (1);
     char var[ A(1) ];
   }
