namespace A
   {
     namespace B
        {
          int a;
          int b;
          int c;
          int d;
          void foo_B();
          namespace B
             {
               int e;
               int f;
               int g;
               int h;
             }
        }
   }


int main()
   {
  // using namespace A;
     using namespace A::B;

  // Can be unparsed as: "A::B::foo_B();" or "foo_B();", but not "B::foo_B();"
  // Bug: unparsed as: "B::foo_B();"
     foo_B();

     a = 5;

     return 0;
  }

