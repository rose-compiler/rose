class Foo
   {
     public:
          virtual void my_foo() { }
   };

class Bar: public Foo
   {
     public:
          void my_foo() { }
   };

typedef void (Foo::*MEM_FUNC)();

void foobar()
   {
  // expected   MEM_FUNC fp1 = (MEM_FUNC)(&Bar::my_foo);
  // result     MEM_FUNC fp1 = &Bar::my_foo;
  // Does NOT work
     MEM_FUNC fp1 = (MEM_FUNC)(&Bar::my_foo);
   }
