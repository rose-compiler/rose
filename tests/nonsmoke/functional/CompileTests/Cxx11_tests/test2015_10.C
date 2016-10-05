// This test code demonstrate where ROSE is, by mistake, generating the same
// function type for both instances of the operator==().  Because of this
// the symbol table is incorrect, or would be if internal assertions didn't
// catch the problem as it was happening.

// template<typename _IntType = int>
class uniform_int_distribution_A
   {
     public:
          struct param_type {};

       // This function type must be different from the one below, because the parameters are different.
          friend bool operator==(const param_type& __p1, const param_type& __p2) { return true; }
   };

// template<typename _IntType = int>
class uniform_int_distribution_B
   {
     public:
          struct param_type {};

          friend bool operator==(const param_type& __p1, const param_type& __p2) { return true; }
   };
