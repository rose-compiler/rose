class Y
   {
     public:
      // This takes a different path in the compiler.
         const double pi_1 = 3.14;

      // This takes the case iek_constant path in parse_structlike()
         const double pi_2;
   };
