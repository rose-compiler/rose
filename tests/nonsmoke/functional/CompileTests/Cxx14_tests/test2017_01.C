// Note that this does not unparse properly (missing trailing "#endif").

#if defined(__INTEL_COMPILER) && __ICC <= 1500
auto a = 0b1000001;  // ASCII (not supported in older Intel compilers).
#else
auto a = 0b100'0001;  // ASCII
#endif

int x;

