const char *g() { return "dynamic initialization"; }
constexpr const char *f(bool p) { return p ? "constant initializer" : g(); }
 
constinit const char *c = f(true); // OK
// constinit const char *d = f(false); // error


