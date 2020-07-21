union u { int a; const char* b; };

u f = { .b = "asdf" };         // OK, active member of the union is b

// DQ (7/21/2020): Failing case should not be tested
// u g = { .a = 1, .b = "asdf" }; // Error, only one initializer may be provided

