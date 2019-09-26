class B
   {
     public:
          typedef int T;
   };

struct D : B
   {
     struct T {};

  // BUG: This us unparsed as: T S; instead of B::T S;
     B::T S;
   };

