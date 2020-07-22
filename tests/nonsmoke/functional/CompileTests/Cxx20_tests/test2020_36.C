struct A { int x; int y; int z; };

// DQ (7/21/2020): Failing case should not be tested
// A a{.y = 2, .x = 1}; // error; designator order does not match declaration order

A b{.x = 1, .z = 2}; // ok, b.y initialized to 0

