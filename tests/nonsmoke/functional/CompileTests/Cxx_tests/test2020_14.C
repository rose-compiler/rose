struct A { int xxxxxx; };

// Original code: A b{.x = 1};
// Generated: struct A b = {.A::x = 1};
A bbbbbb{.xxxxxx = 42};

