class A;

int (A::*pf)();
int (A::*pcf)() const;
int (A::*pvf)() volatile;
int (A::*pcvf)() const volatile;

int (A::*pflr)() &;
int (A::*pfrr)() &&;
