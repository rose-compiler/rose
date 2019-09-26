// struct A {};

struct X
   {
     struct Y {};

     int Y::* Y::* Y::* Y::* *p1 = 0L;
   };

struct A
   {
     struct B {};

     int B::* B::* B::* B::* *p2 = 0L;
   };

struct M
   {
     struct N {};

     int N::* N::* N::* N::* *p2 = 0L;
   };

struct P
   {
     struct Q {};

     int Q::* Q::* Q::* Q::* *p4 = 0L;
   };

void foobar()
   {
     int A::B::* M::N::* A::B::* P::Q::* *p3 = 0L;
   }
 
