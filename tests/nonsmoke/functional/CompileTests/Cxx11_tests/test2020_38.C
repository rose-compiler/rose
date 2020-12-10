
class A;

int (A::*pf)();
int (A::*pcf)() const;
int (A::*pvf)() volatile;
int (A::*pcvf)() const volatile;

int (A::*pflr)() &;
int (A::*pfrr)() &&;

class A
   {
     public:
          int f()                  { return 42; }
          int fc() const           { return 42; }
          int fv() volatile        { return 42; }
          int fcv() const volatile { return 42; }

          int (flr)() &  {return 0; };
          int (frr)() && {return 0; };

          A() { }
          A(int ii) { }
   };


void foobar()
   {
     A x;
     pf = &A::f;
     (x.*pf)();
     const A cx (0);
     pcf = &A::fc;
     (cx.*pcf)();
     volatile A vx;
     pvf = &A::fv;
     (vx.*pvf)();
     const volatile A cvx (0);
     pcvf = &A::fcv;
     (cvx.*pcvf)();
   }

