// structured bindings

struct S { int x1 : 2; volatile double y1; };
S f();
const auto [ x, y ] = f();

