

// void X::foobar (int i){ }

int main()
   {
     class X
        {
          public:
               int abc;
               void foobar (int i){ };
            // void foobar (int i);
        };

     X y;
     y.foobar(0);
     return 0;
   }
