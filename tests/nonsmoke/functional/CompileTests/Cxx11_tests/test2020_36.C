
struct A 
   {
     int foo() & { return 5; }
     int foo()&& { return 7; } // rvalue
   };

