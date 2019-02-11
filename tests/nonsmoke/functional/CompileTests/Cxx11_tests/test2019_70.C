class B_
   {
     public:
          typedef int T;
          enum color { R, O, Y };
          struct S { T i; };
          S b;
          B_(T t) { b.i = t; }
   };

struct D_ : B_
   {
     typedef char color;
     struct T { B_::color c; };
     B_::T S;
     T b;
     D_(int s) : B_(s+1), S(s-1) { b.c = R; }
   };

void foobar()
   {
     D_ d(4);
  // ieq(d.B_::b.i, 5);
  // ieq(d.b.c, B_::R);
   }
