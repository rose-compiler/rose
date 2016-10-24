#if 0
_Alignas, _Alignof, max_align_t, stdalign.h
_Atomic, stdatomic.h
_Generic
_Noreturn, stdnoreturn.h
_Static_assert
_Thread_local
#endif

// stdatomic.h does not apear to be available within GNU 4.8 (and so is not available to EDG).
// It is not possible to support this header file with GNU currently.
// #include<stdatomic.h>

