namespace A
   {
     void foo();
     void foo_A();
     namespace B
        {
          void foo();
          void foo_B();
          namespace B
             {
               void foox();
               void foo_C();
             }
          using namespace B;
          void foox()
             {
               foo_C();
             }
        }

  // Either of these may be defined, but not both! It is an error, but I'm not sure why.
  // using namespace B;
     using namespace B::B;
   }


int main()
   {
     using namespace A;
     using namespace A::B;
     foo_A();
     foo_B();
     foo_C();
     A::foo_C();

     return 0;
  }

