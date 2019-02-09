struct B { int rhs_variable; };

class D : public B
   {
     public:
          void foo()
             {
               int D::*lhs_variable = &D::rhs_variable;
             };
   };

