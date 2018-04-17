

class X
   {
     public:
          X(int) {};
   };

class Y
   {
     public:
          Y(X) {};
   };

class Z
   {
     public:
          Z() {};
          Z(Y) {};
   };

void foo()
   {
     Z z;
     z = Z(Y(X(1)));
   }

