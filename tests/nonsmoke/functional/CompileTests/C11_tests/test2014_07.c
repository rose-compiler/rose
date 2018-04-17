
#ifdef __INTEL_COMPILER
void func (); //C11, func never returns (no such specification that I know of for Intel compiler).
#else
_Noreturn void func (); //C11, func never returns
#endif
