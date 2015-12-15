class A
   {
     public:
          virtual void f() throw ( int ) { }
   };

class B: public A
   {
     public:
       // Note that this SHOULD be an error.
          void f() throw ( int, double ) { }
   };
