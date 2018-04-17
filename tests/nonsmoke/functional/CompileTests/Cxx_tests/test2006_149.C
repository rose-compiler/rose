// This test code does not completely represent a bug

// This code compiles as both C and C++ code independent of the setting of this macro.
#define B_IS_DEFINED 0

typedef struct a {
    struct b *c;
} d;

#if B_IS_DEFINED
// Add something to define "b"
struct b
   {
     int x;
   } *e;
#endif

void foo()
   {
#if B_IS_DEFINED
     e->x = 42;
#endif
     d varD;
     varD.c = 0L;

#if __cplusplus
     b* varB;
#else
     struct b* varB;
#endif
   }



