
// C++11
   [=]() -> some_type { return foo() * 42; } // ok
   [=]                { return foo() * 42; } // ok, deduces "-> some_type"
