

class A
   {
     public:
          A () {}
          A (float f, double d) {}
   };

class B : public A
   {
     public:
          B (float f,double d) : A(f,d) {}      
          B (double d) : A() {}      
   };

void foo()
   {
     B b(1,2);
   }
