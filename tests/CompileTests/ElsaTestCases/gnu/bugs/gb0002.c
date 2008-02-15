// gb0002.c
// applying type specifier keywords to a typedef

// EDG and Elsa both reject this; it's in the regression test
// directory just to have a name to attach to this bug.

typedef unsigned int uint32_t;
typedef uint32_t unsigned long;
