namespace A
   {
     void foo();
   }

void foo();
void foobar()
   {
     foo();
     A::foo();
   }
