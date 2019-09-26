struct A
   {
     int i;
     A(int);
   };

struct X_
   {
     struct Y_
        {
          int i, j;
          int A::* pm;
          int A::*const cpm;
          Y_(int a, int b, int A::* q) : i(a), j(b), pm(q), cpm(q) { }
        };
   };


void foobar()
   {
     A o(18);
     X_::Y_ y(5, 3, &A::i);
     int X_::Y_::* *p1 = new int X_::Y_::* (&X_::Y_::j);
     y.*(*p1);
     int A::* const X_::Y_::* *p2 = new int A::* const X_::Y_::* (&X_::Y_::cpm);
     o.*(y.*(*p2));
   }
 
