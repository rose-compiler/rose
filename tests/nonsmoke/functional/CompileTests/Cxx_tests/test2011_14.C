/*
In the attached case, the preprocessor directive #endif is not unparsed unless there is a newline present after it.
In the absence of a newline, attempting to compile the attached file with ROSE produces:



identityTranslator -c /preprocessorBug.C 
"preprocessorBug.C", line 7: warning: last
          line of file ends without a newline
  #endif
        ^

rose_preprocessorBug.C:1: error: unterminated #if

*/

#if 0

class C
{
}

// Make sure there is NOT a newline after the #endif.
#endif
