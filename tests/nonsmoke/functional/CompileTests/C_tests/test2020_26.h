// RC-68: (need to be split between header and source files.

typedef struct {} X;

X * bar (X *, int);

// This appears to be a recursive macro (might have to implement option to 
// generate *.i and *.ii intermediate files to avoid re-expansion of macros.
#define bar(s) bar (s, 0) 

