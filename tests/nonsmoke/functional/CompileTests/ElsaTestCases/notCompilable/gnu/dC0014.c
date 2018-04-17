// dC0014.c
// combining a typedef-name with a type modifier like "long"

// from the kernel

// sm: this is actually a bug in gcc-2 (and vestiges remain in gcc-3)

typedef unsigned int u32;
u32 long off;
typedef u32 long off2;

typedef u32 long;
typedef u32 unsigned long;


// ----------------------------------------
// copied from (now defunct) in/c/k0001.c

// the input was from e2fsprogs, though e2fsprogs-1.36 does
// not seem to have this problem anymore

typedef int fooint;

// interestingly, of the three uses of the "feature" in this file,
// this one is the only one accepted by gcc-3
typedef fooint long /*omitted declarator*/;

typedef fooint long blah;


