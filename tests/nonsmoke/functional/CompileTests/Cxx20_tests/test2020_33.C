consteval int f() { return 42; }
consteval auto g() { return &f; }
consteval int h(int (*p)() = g()) { return p(); }
constexpr int r = h();   // OK

// DQ (7/21/2020): Failing case should not be tested
// constexpr auto e = g();  // ill-formed: a pointer to an immediate function is
                            // not a permitted result of a constant expression

