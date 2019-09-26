class MyClass
   {
     public:
          MyClass(int);
          MyClass(MyClass &);
          MyClass(const MyClass &);
          MyClass(volatile MyClass &);
          MyClass(const volatile MyClass &);
          int class_member;
   };

MyClass::MyClass(int i) : class_member(i) { }
MyClass::MyClass(MyClass &x) : class_member(42 * x.class_member) { }
MyClass::MyClass(const MyClass &x) : class_member(42 * x.class_member) { }
MyClass::MyClass(volatile MyClass &x) : class_member(42 * x.class_member) { }

// Bug: "const volatile" is unparsed as "volatile"
MyClass::MyClass(const volatile MyClass &x) : class_member(42 * x.class_member) { }

int foo(MyClass x) { return x.class_member; }

void foobar()
   {
     MyClass variable(42);
     const MyClass const_variable(42);
     volatile MyClass volatile_variable(42);

  // Bug: "const volatile" is unparsed as "volatile"
     const volatile MyClass const_volatile_variable(42);
   }

