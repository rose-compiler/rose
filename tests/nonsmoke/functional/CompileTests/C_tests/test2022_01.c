// New test as a variant of test2004_126.c for ClangFE support.
// This is to show if declared_in_function_prototype in EDG is set. 

enum x{FALSE, TRUE} ;

int
f ( enum x b )
   {
     return FALSE;
   }

