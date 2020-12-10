class A {};

class B: public A
   {
     public:
          B (double x);
   };

class C;

class D
   {
     public:
          D (const C & x);
   };


class C
   {
     public:
     C (A x, A y, double z = 1.0);
   };

void foobar()
   {
     double array[3];

     D value = C(B(array[0]),B(array[1]), array[2]);
   }

