void foo_A();
void foo_B();
void foo_C();
void foo_D();

void foo_A()
   {
     foo_B();
   }

void foo_B()
   {
   }

void foo_C()
   {
     foo_B();
   }

void foo_D()
   {
     foo_B();
     foo_C();
   }

int main()
   {
     foo_A();
     foo_B();
     foo_C();
     foo_D();

     return 0;
   }
