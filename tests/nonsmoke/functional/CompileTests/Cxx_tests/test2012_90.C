class Y
   {
     public:
   // DQ (4/17/2014): I had to comment out the initialization of pi_1(3.14) again for EDG 4.8.
   // DQ (3/25/2014): EDG now acts exactly like GNU in this respect (initialization of pi_1 added).
   // Y() : pi_1(3.14), pi_2(2.71)  {}
      Y() : pi_2(2.71)  {}

      // DQ (3/25/2014): EDG is backward compatable and still allows in class initialization).
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

