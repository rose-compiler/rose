/*
Hi Dan,

I've been trying to get the gcc SPEC benchmark compiling w/ ROSE. It's
been segfaulting on me, but I've been able to narrow the problem down
to the situation described in the testcase below.  I'm using  ROSE
0.8.10a.  I've succesfully gotten 12 out of 21 SPEC benchmarks passing
through ROSE by the way.


stonea@monk ~/ma/rose/gccErr
$ cat testCase.c
enum Constants { constA, constB, constC };

int bar() { return 1; }

void foo() {
   bar() ? constA : ((enum Constants) 2);
}


stonea@monk ~/ma/rose/gccErr
$ gcc -c testCase.c

stonea@monk ~/ma/rose/gccErr
$ rosec -rose:C_only -c testCase.c
Segmentation fault


Thanks,
-Andy 
*/


enum Constants { constA, constB, constC };

int bar() { return 1; }

void foo() {
   bar() ? constA : ((enum Constants) 2);
}

