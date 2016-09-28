_Static_assert(1,"Not a standard compliant compiler");

// This is not allowed (must be a string literal)
// const char* foo();
// _Static_assert(1,foo());
