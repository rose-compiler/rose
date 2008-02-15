// cc.in15
// ctors and dtors

class Foo {
public:
  // member with 'const' qualifier
  //int f() const;

  // constructor
  Foo();

  // constructor with arguments
  //Foo(int x);

  // and inline definition
  ///*explicit*/ Foo(int x) { x+5; }

  // destructor
  ~Foo();

};


// out-of-line ctor
Foo::Foo()
{
  5;
}


// out-of-line dtor
Foo::~Foo()
{
  8;
}


void f()
{
  Foo f;
  
  //ERROR(1): Foo g(3);  // error
}
