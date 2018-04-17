// The problem with this code is that the function definition is 
// placed into the global scope instead of the class definition's scope

// int x;

class X
   {
     public:
//        int abc;
          void foobar (int i){ };
//        void foobar (int i);
   };

// void X::foobar (int i){ }

#if 0
int main()
   {
     x = 42;
     X y;
     y.foobar(x);
     return 0;
   }
#endif
