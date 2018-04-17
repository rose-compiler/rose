class X
   {
     public:
         int x;
         int foo() { return x; }
   };


int main()
   {
     X *xPtr = 0;
     xPtr->foo();

     return 0;
   }
