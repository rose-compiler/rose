
typedef int some_type;

some_type foo();

void foobar()
   {
  // C++11
     [=]() -> some_type { return foo() * 42; }; // ok

     [=]                { return foo() * 42; }; // ok, deduces "-> some_type"
   }
