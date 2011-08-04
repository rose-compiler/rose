namespace A
   {
     void foo();
     namespace B
        {
          void foo();
        };
   };

void foo();

// This member function declaration requires name qualification.
void A::foo()
   {
   }

void A::B::foo()
   {
      foo();
      A::foo();
      ::foo();
   }
