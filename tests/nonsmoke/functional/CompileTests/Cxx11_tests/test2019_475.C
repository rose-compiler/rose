
class A
   {
     public:
          void foo() { xxx = 0; };

       // Bug (generated): int A::xxx;
       // Original code:   int xxx;
          int xxx;
   };
