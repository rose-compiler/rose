#include "rose.h"

// Intercept call for functions:

// void ATinit(int argc, char *argv[], ATerm *bottomOfStack)
#define ATinit(argc,argv,bottomOfStack) rose_ATinit((argc),(argv),bottomOfStack)

// ATerm ATreadFromTextFile(FILE *file)
#define ATreadFromTextFile(F) rose_ATreadFromTextFile(F)


// This is a subject called "Varadic Macros" and a extra comma is output when the last macro 
// element is empty, so we need to let "const char *pattern" be a part of the "..." part.
// ATbool ATmatch(ATerm t, const char *pattern, ...)
// #define ATmatch(X,Y) rose_ATmatch((X),(Y))
#define ATmatch(X,...) rose_ATmatch(X,__VA_ARGS__)

// ATerm rose_AT_getAnnotations(ATerm t)
#define AT_getAnnotations(X) rose_AT_getAnnotations(X)

#if 0
// void rose_ATinit(int argc, char *argv[], rose_ATerm *bottomOfStack);
void rose_ATinit(int argc, char *argv[], void *bottomOfStack);

// rose_ATbool rose_ATmatch(rose_ATerm t, const char *pattern, ...);
// rose_ATbool rose_ATmatch(ATerm t, const char *pattern, ...);
bool rose_ATmatch(ATerm t, const char *pattern, ...);

ATerm rose_AT_getAnnotations(ATerm t);
#endif

// #include "rose_aterm1.h"
// #include "rose_aterm2.h"

#include "roseSpecificCode.h"

