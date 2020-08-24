namespace A
   {
     namespace B
        {
       // int a;
          void foo_B();
          namespace B
             {
             }
        }
   }


int main()
   {
     using namespace A;
     using namespace A::B;

  // Can be unparsed as: "A::B::foo_B();" or "foo_B();", but not "B::foo_B();"
  // Bug: unparsed as: "B::foo_B();"
     foo_B();

  // a = 5;

     return 0;
  }

