// Names Used in Assembler Code
// specify the name to be used in the assembler code for a C function or variable by writing the asm (or __asm__) keyword after the declarator
int foo asm ("myfoo") = 2;

extern int func () asm ("FUNC");

#if 0
// This is not C++
func (x, y)
   int x, y;
#endif
