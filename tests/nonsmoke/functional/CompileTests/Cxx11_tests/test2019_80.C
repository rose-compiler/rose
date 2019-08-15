struct B { int rhs_variable; };

class D : public B
   {
     public:
          void foo()
             {
            // Name qualification is required for bi.
            // BUG: unparsed as: "&rhs_variable" instead of "&B::rhs_variable"
               int B::*lhs_variable = &B::rhs_variable;
             };
   };

