
// ROSE-1898: (C++11) Explicit & default constructors

struct piecewise_construct_t 
   {
     explicit piecewise_construct_t() = default;
   };

constexpr piecewise_construct_t piecewise_construct = piecewise_construct_t();


