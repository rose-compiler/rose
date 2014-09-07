struct S 
   {
  // constexpr S(int);
     constexpr S(int) {};
   };

// constexpr S::S(int x) { return 5; };

const S s0(0);
constexpr S s1(1);

