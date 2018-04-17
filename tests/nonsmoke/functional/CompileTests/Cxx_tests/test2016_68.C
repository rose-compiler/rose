class Base
   {
     public:
          virtual bool func1();
   };

class Derived2 : public Base
   {
     public:
          virtual bool func1();
       // virtual bool funcDer2();
   };

Base* pDer2 = new Derived2;

Derived2* pDerCasted = dynamic_cast<Derived2*>(pDer2);
