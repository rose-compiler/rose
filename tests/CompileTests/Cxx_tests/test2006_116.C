#if 0
// Reported by Liao

Dan,
It seems that ROSE secretly changes the pointer pointing outside of
underlying object after issuing a warning. The problem came from the
EP benchmark from NAS parallel benchmark suite, which has unverified
execution result due to this bug.

I wish I could find this bug earlier. But the warning message is
deeply buried in other warning messages and I painfully found the bug
by comparing the translated code to its original one line by line.

Here is the details:

bash-2.05b$ cat inputBug327.c
#include <stdio.h>
extern void foo(double y[]);
double mm[10];
int main(void)
{
//double mm[10]; // no warning and no changes at all if it is local scope, ??
 foo(mm-1); // looks a very strange argument passing, but actually
used in the benchmark.
 return 0;
}

bash-2.05b$ addVariableDeclaration -c inputBug327.c
"inputBug327.c", line 13: warning: pointer points outside of underlying object
   foo(mm-1);

         ^
---------------------------------------
bash-2.05b$ cat rose_inputBug327.c
#include <stdio.h>
extern void foo(double y[]);
double mm[10];

int main()
{
 int newVariable;
//double mm[10];
 foo(mm);  // OOps, -1 got lost!! unverified result of course!
 return 0;
}

If this bug gets fix, we can roughly claim that 7/8 NAS benchmarks
have passed then. By roughly, I mean I still have problem in
preserving the preprocessing information located at the end of a
scope. Current workaround is to comment them out.
#endif

#include <stdio.h>
extern void foo(double y[]);
extern void foobar(double* y);
double mm_lowerBound;
double mm[10];
int main(void)
{
//double mm[10]; // no warning and no changes at all if it is local scope, ??
 mm[7] = 12;
 foo(mm-1); // looks a very strange argument passing, but actually used in the benchmark.
 foobar(mm+1);
 foo((mm+1)-1);
 foo((mm+1)-2);

// ERROR: Using EDG 3.3, this is unparsed as "foo(mm);"
 foo((mm-1)-2);

// ERROR: Using EDG 3.3, this is unparsed as "mm_lowerBound = (mm[0]);" 
 mm_lowerBound = (mm-1)[0];

// ERROR: Using EDG 3.3, this is unparsed as "mm[0] = 3.14;" 
 (mm-1)[0] = 3.14;

// This is unparsed correctly!
 ((mm+1)-2)[0] = 0.0;
 return 0;
}
