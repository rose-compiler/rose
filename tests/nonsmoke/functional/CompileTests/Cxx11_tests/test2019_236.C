struct A
   {
     int b;
     union { int c; };
   };

void foobar()
   {
     auto d = &A::c;
   }
