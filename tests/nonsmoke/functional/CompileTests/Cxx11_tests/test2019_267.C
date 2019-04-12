class L
   {
     public:
          L(int i) : li(i) { }
          int li;
   };

class A : public L
   {
     public:
          A(int i) : L(i * 2), ai(i) { }
          int ai;
   };

class B : public L
   {
     public:
          B(int i) : L(i / 2), bi(i) { }
          int bi;
   };

class C : public A, public B
   {
     public:
          C(int i) : A(i + 3), B(i - 3), ci(i) { }
          int ci;
          void f();
   };

void C::f()
   {
     A::li;
     B::li;
   }

