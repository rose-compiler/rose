
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


DQ (9/24/2011): After fixing the AST so that constant folded values can be by default
    replaced by the original expression trees, the follwoing two codes faile to merge.
    The work also restricted the AST traversal to NOT traverse the original expression
    trees.  An option in the frontend allows the the original expression trees to be
    removed from the AST; leaving the constant folded values.  However the default is
    to replace the constant folded values with the original expression trees (since ROSE
    is focused on source-to-source).  
    The following test codes now file (our of hundreds that are tested):
         mergeTest_90.C
         mergeTest_124.C
    Each of these used to pass, each now file on what appears to be the same error.

