struct A { int x; };

// Original code: A b{.x = 1};
// Generated: struct A b = {.A::x = 1};
A b{.x = 1};

