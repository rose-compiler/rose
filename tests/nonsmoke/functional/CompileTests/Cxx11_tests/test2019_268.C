class L
   {
     public:
          int li;
   };

class A : public L
   {
   };

class B : public L
   {
   };

class C : public A, public B
   {
     public:
          void f();
   };

void C::f()
   {
     A::li;
     B::li;
   }

