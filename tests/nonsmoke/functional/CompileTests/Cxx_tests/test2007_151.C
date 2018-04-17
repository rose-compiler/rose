// Examples of strange stuff from Dawson Engler's talk

#if 0
// Example 1: (not allowed by EDG)
void a;
#endif

#if 0
// Example 2: (not allowed by EDG)
short x;
int *y = &(int)x;
#endif

#if 0
// Example 3: (not allowed by EDG)
int foo(int a, int a);
#endif

#if 0
// Example 4: (not allowed by EDG)
unsigned x @ "TEXT";
#endif

#if 0
// Example 5: (not allowed by EDG)
// unless "-packed"!
__packed (...) struct foo {...}
#endif

// Example 6:
unsigned x = 0xdeadbeef;

// Example 7:
char* strange_end_of_line = "abc\r";

#if 0
// Example 8: (not allowed by EDG)
#pragma asm
mov eax, eab
#pragma end_asm
#endif

#if 0
// Example 9: (not allowed by EDG)
asm foo()
   {
     mov eax, eab
   }
#endif

#if 0
// Example 10:  (not allowed by EDG)
// newline = end
__asm mov eax, eab
#endif

#if 0
// Example 11: (not allowed by EDG)
// "]" = end
__asm [
    mov eax, eab
                 ]
#endif

