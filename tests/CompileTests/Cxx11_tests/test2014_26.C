// auto f(int p, decltype(p)) -> decltype(p);
// auto foobar(int p) -> int;
auto f(int p) -> decltype(p);
