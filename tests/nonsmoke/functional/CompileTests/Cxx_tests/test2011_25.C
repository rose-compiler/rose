// test for hiding class name

typedef int B;

class A
   {
     public:
          int a;
          A() {}

          void foo()
             {
               int A;
               double B;
            // A* b;  // would cause an error because of overwritten class A by int A !!!! -> so ok, when int A hides constructor & class A
               class A* b;  // would cause an error because of overwritten class A by int A !!!! -> so ok, when int A hides constructor & class A
             }
   };
