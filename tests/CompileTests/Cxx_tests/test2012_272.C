// Test code from copyAST_tests/copytest2007_12.C

// Test code to test use of explicit template declarations

// template <typename T> T min (T a, T b) { return a < b ? a : b; }
// template <typename T> T min (T a, T b) { return a; }
template <typename T> void min (T a) {}

// Explicit instantiation of templated function
// template int min<int>(int x, int y);
template void min<int>(int x);
