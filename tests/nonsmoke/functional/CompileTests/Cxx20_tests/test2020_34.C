const char *g() { return "dynamic initialization"; }

constexpr const char *f(bool p) { return p ? "constant initializer" : g(); }

// DQ (7/21/2020): constinit appears to not be implemented in EDG 6.0.
constinit const char *c = f(true); // OK

// DQ (7/21/2020): Failing case should not be tested
// constinit const char *d = f(false); // error


