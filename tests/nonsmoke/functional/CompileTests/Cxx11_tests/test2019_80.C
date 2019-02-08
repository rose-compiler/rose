struct B { int rhs_variable; };

class D : public B
   {
     public:
          void foo()
             {
            // Name qualification is required for bi.
            // BUG: unparsed as: "&bi" instead of "&B::bi"
               int B::*lhs_variable = &B::rhs_variable;
             };
   };

