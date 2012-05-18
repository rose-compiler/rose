class Y
   {
     public:
         Y() : pi_2(2.71)  {}
         
      // This takes a different path in the compiler.
         const double pi_1 = 3.14;

      // This takes the case iek_constant path in parse_structlike()
         const double pi_2;
   };

void foo()
   {
     Y y;

  // We want: "y.pi_1;" so we need to use the unfolded expression tree.
     double yy = y.pi_1;
   }

