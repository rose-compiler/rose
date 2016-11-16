
typedef int some_type;

some_type foo();

struct BAR
   {
     int baz(int);
   };

BAR bar;

  // C++11, explicitly named return type
     some_type f1()         { return foo() * 42; }; // ok
     auto f2() -> some_type { return foo() * 42; }; // ok

  // C++14
     auto f3()              { return foo() * 42; }; // ok, deduces "-> some_type"
     auto g() {                                // ok, deduces "-> some_type"
        while( true ) {
            if( true ) {
                return foo() * 42;            // with arbitrary control flow
            }
        }
        return bar.baz(84);                   // & multiple returns
    };                                        //   (types must be the same)

void foobar()
   {

   }
