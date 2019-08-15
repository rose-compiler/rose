struct B { int bi; };

class D : private B
   {
     public:
          int D::*bar(int B::*p) const;

          void foo()
             {
               bar(&B::bi);
             };
   };

