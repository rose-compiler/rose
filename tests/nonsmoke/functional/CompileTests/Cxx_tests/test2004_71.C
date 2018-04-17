namespace A
   {
     namespace B
        {
          class classInNamespace
             {
               public:
               int x;
               int foobar();
             };
        }
   }

int main()
   {
     A::B::classInNamespace X;
     return 0;
   }
