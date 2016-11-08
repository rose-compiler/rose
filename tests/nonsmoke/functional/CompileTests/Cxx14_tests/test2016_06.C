
 // C++11, explicitly named return type
    some_type f()         { return foo() * 42; } // ok
    auto f() -> some_type { return foo() * 42; } // ok
    // C++14
    auto f()              { return foo() * 42; } // ok, deduces "-> some_type"
    auto g() {                                // ok, deduces "-> some_type"
        while( something() ) {
            if( expr ) {
                return foo() * 42;            // with arbitrary control flow
            }
        }
        return bar.baz(84);                   // & multiple returns
    }                                         //   (types must be the same)
