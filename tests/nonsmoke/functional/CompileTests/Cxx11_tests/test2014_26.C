// This is the new function syntax, which we unparse as the old function syntax currently.

// auto f(int p, decltype(p)) -> decltype(p);
// auto foobar(int p) -> int;
auto f(int p) -> decltype(p);
