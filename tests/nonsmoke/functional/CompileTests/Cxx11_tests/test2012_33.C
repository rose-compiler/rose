// Example C++ 11 features:
//    Translate "auto x = vec.iterator();" into "std::vector<Foo>::iterator x = vec.iterator();"

// Translate:
auto x = vec.iterator();
// to:
std::vector<Foo>::iterator x = vec.iterator();


