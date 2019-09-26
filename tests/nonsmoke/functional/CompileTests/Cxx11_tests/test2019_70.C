class B
   {
     public:
          typedef int T;
          enum color { R, O, Y };
          struct S { T i; };
          S b;
          B(T t) { b.i = t; }
   };

struct D : B
   {
     typedef char color;
     struct T { B::color c; };
     B::T S;
     T b;
     D(int s) : B(s+1), S(s-1) { b.c = R; }
   };

void foobar()
   {
     D d(4);
   }
