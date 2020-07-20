consteval int f() { return 42; }
consteval auto g() { return &f; }
consteval int h(int (*p)() = g()) { return p(); }
constexpr int r = h();   // OK
constexpr auto e = g();  // ill-formed: a pointer to an immediate function is
                         // not a permitted result of a constant expression

