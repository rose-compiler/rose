// Example of C11 atomic use (not yet supported in EDG).
#if __GNUC__ == 4 && __GNUC_MINOR__ > 8 || __GNUC__ > 4
_Atomic(int) counter;
#endif
