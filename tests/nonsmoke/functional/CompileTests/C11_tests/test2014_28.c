void quick_exit (int __status) __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));

#ifdef __INTEL_COMPILER
void foobar(int x); //C11, func never returns (no such specification that I know of for Intel compiler).
#else
_Noreturn void foobar(int x);
#endif
