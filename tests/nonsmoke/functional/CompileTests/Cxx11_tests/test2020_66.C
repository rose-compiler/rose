
class A
   {
     protected:
          virtual void foo1(void) {}
          char* b;
   };

class B : public A
   {
     public:
          B();
          virtual void B::foo1();
   };

class C
   {
     public:
          friend void B::foo1(void);
   };

