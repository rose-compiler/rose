
// This code demonstrates a simple error in how the scope is explicitly set (incorrectly)
// for a function's local variable.

// #include<string>

int x;

void foo()
   {
     int y;
   }


class X;
class Y
   {
     friend class X;
     public:
          typedef X Y1;
   };

class X
   {
     int i;
   };


Y::Y1 x1;
X x2;

