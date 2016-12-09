//////////////// preparation for member function pointer cast /////////////////
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
#if 0
  //////////// cast of member function pointer ////////////
  MEM_FUNC fp = static_cast<void (Foo::*)()>(&Bar::my_foo);
  // This issue is that we are missing the cast!
  // expected   MEM_FUNC fp = static_cast<void (Foo::*)()>(&Bar::my_foo);
  // result     MEM_FUNC fp = &Bar::my_foo;
  // Does NOT work
#endif

#if 1
  // DQ (8/19/2016): I think the problem is that the unfolded expression should 
  // be used by default but instead the folded expression is being used instead 
  // (so we loose the cast expression).
  MEM_FUNC fp1 = (MEM_FUNC)(&Bar::my_foo);
  // This issue is that we are missing the cast!
  // expected   MEM_FUNC fp1 = (MEM_FUNC)(&Bar::my_foo);
  // result     MEM_FUNC fp1 = &Bar::my_foo;
  // Does NOT work
#endif
}
