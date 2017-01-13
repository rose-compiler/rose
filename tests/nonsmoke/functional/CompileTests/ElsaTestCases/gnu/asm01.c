// asm01.c
// testing Asm Labels

// http://gcc.gnu.org/onlinedocs/gcc-3.1/gcc/Asm-Labels.html

// You can specify the name to be used in the assembler code for a C
// function or variable by writing the asm (or __asm__) keyword after
// the declarator as follows:

int foo asm ("myfoo") = 2;

// Where an assembler name for an object or function is specified (see
// Asm Labels), at present the attribute must follow the asm
// specification

int foo2 asm ("myfoo2") __attribute__((blah)) = 3;

// this also works for function decls

int foo3(int, float) asm("myfoo3");
int foo4(int, float) asm("myfoo4") __attribute__((blah));

