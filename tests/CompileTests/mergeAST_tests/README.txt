
This directory tests the AST merge mechanism in ROSE.

Interesting problem in testing mergeTest_06.C
   1) This file has only one line of code: "#include<string>"
   2) if running "g++ -E" on the file we visit the pthreads support in ROSE
      and cause code to be 
   3) Then add: 
#define __attribute__(X)
#define __sync_fetch_and_add(X,Y) ((_Atomic_word) 0)
to the top of the file.

   4) g++ -E -D_GLIBCXX_GTHREAD_USE_WEAK=0 <file>

Current problem test:
   ./testMerge -rose:astMerge -c /home/dquinlan/ROSE/git-dq-main-rc/tests/CompileTests/mergeAST_tests/mergeTest_137.C


