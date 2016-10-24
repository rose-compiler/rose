// Example C++ 11 features:
//    Translate "int x; int y = x + 1;" into "int x; decltype(x) y = x + 1;".

// Translate:
int x; int y = x + 1;
// to:
int x; decltype(x) y = x + 1;
