// Example C++ 11 features:
//    Translate "int x; int y = x + 1;" into "int x; decltype(x) y = x + 1;".

// Translate:
int x1; int y1 = x1 + 1;
// to:
int x2; decltype(x2) y2 = x2 + 1;
