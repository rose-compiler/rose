struct B { int bi; };

class D : private B
   {
     public:
          void foo()
             {
               int D::*lhs_variable;
               bool var = (lhs_variable == &D::bi);
             };
   };

