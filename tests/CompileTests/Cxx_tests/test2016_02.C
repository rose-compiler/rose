// Examnple of member function pointers used at template arguments (non-type template arguments).

template <class Bar,
          class Baz,
          class BazReturnType,
          void (Bar::*BarSetterFunction)(const BazReturnType &),
          BazReturnType (Baz::*BazGetterFunction)(void) const>
class Foo
   {
     public:
          Foo( Bar *bar ) : m_bar(bar)
             {
             }

          void FooMemberFunction( const Baz *baz )
             {
            // boost::bind( BarSetterFunction, m_bar, boost::bind( BazGetterFunction, baz )() ) ();
             }

          Bar *m_bar;
   };

// This template is instantiated and used in the library depending on the types of Bar and Baz like so:

typedef int BazReturnType;
typedef int ReturnTypeFromBazGetterFunction;

class MyBar { public: void ActualSetterFunction (const BazReturnType &) {} };
class MyBaz { public: BazReturnType ActualGetterFunction (void) const {} };


typedef Foo<MyBar, MyBaz, ReturnTypeFromBazGetterFunction, &MyBar::ActualSetterFunction, &MyBaz::ActualGetterFunction > MyFoo;

int foobar()
   {
     MyBar *bar = new MyBar;
     MyBaz *baz = new MyBaz;
     MyFoo *f = new MyFoo( bar );

  // This compiles with ROSE (EDG), but not g++ (the point is to put the statement into a function instead of lobal scope).
  // f->FooMemberFunction( baz );

     f->FooMemberFunction( baz );
   }
