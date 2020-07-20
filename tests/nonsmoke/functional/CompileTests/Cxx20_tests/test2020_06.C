union u { int a; const char* b; };
u f = { .b = "asdf" };         // OK, active member of the union is b
u g = { .a = 1, .b = "asdf" }; // Error, only one initializer may be provided
