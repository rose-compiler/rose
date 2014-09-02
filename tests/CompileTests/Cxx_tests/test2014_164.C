//////////////////// preparation for function pointer cast ////////////////////
void foo(int)
{ }

void foo(int, int)
{ }

typedef void(*Func1)(int);
typedef void(*Func2)(int,int);

//////////////// preparation for member function pointer cast /////////////////
class Foo{

public:
  virtual void my_foo()
  { }
};

class Bar: public Foo{

public:
  void my_foo()
  { }
};

typedef void (Foo::*MEM_FUNC)();

//////////////////// preparation for variable pointer cast ////////////////////
class Base {};
class Derived: public Base {};

int main()
{
#if 0
  //////////////// cast of function pointer ////////////////
  Func1 func1 = static_cast<Func1>(&foo);
  // expected   Func1 func1 = static_cast<Func1>(&foo);
  // result     Func1 func1 = (Func1 )(&foo);
  // works (however, static cast is reinterpreted as c cast)

  Func2 func2 = (Func2)(&foo);
  // expected   Func2 func2 = (Func2)(&foo);
  // result     Func2 func2 = (Func2 )(&::foo);
  // works
#endif

  //////////// cast of member function pointer ////////////
  MEM_FUNC fp = static_cast<void (Foo::*)()>(&Bar::my_foo);
  // expected   MEM_FUNC fp = static_cast<void (Foo::*)()>(&Bar::my_foo);
  // result     MEM_FUNC fp = &Bar::my_foo;
  // Does NOT work

  MEM_FUNC fp1 = (MEM_FUNC)(&Bar::my_foo);
  // expected   MEM_FUNC fp1 = (MEM_FUNC)(&Bar::my_foo);
  // result     MEM_FUNC fp1 = &Bar::my_foo;
  // Does NOT work

#if 0
  //////////////// cast of variable pointer ////////////////
  Base *a = new Base;
  // expected   Base *a = new Base;
  // result     class Base *a = new Base;
  // works

  Derived *b = static_cast<Derived*>(a);
  // expected   Derived *b = static_cast<Derived*>(a);
  // result     class Derived *b = (class Derived *)a;
  // works (however, static cast is reinterpreted as c cast)
#endif

  return 0;
}
