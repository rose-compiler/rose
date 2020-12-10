
struct X
   {
     static const int abc = 42;
   };

// Unparsed as extern const int X::abc; which is an error for clang, but not GNU.
const int X::abc;

