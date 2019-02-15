class B
   {
     public:
          typedef int T;
          struct S { };
          B(T t);
   };

struct D : B
   {
     struct T { };
     B::T S;
     D(int s) : B(s+1), S(s-1) { }
   };

