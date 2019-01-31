
class A
   {
     public:
          virtual int foobar();
          int i;
     public:
          int foo();
          A();
   };


class B : public A
   {
     public:
       // int abc;
       // This is an old style using-declaration.
       // BUG: This is unparsed without the public access level.
          A::foo;
          B();
     private:
          int foobar();
   };


void myFunction()
   {
     B o;
     o.foo();
   }

