
class A
   {
     public:
          A(int i);
   };

class B : public A
   {
     public:
          B() : A(1) {}
   };

