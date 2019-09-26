struct B { int bi; };

class D : private B
   {
     int di;
     public:
          void foo()
             {
               if (bar(&B::bi) == &D::bi);
               if (&(this->*bar(&B::bi)) == &bi);
             };

          int D::*bar(int B::*p) const { return p; }
   };

void foobar()
   {
     D d;
     d.foo();
   }
