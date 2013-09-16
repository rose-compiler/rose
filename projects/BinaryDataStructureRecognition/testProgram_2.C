class A
   {
     public:
          int x;
          int y;
   };

class B
   {
     public:
       // This will not be found by our matching since the value is not SgAsmTypeDoubleWord
          short x;
   };

int x;

int main()
   {
     A a;

     x = 0xcafecafe;

     a.x = 0xcafe0000;
     a.y = 0xcafe0001;
     a.x = 0xcafe0002;
     a.x = a.y;

     char c = 'A';

     unsigned long x = 0xcafeffff;

     B b;

     b.x = 0xcafe;

     return 0xcafe00ff;
   }
