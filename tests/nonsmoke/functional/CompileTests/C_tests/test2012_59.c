// This file demonstrates an error in the output of the final "#endif"
// of code that is not used (the macro "TESTING" is not defined).
// From: wget-1.14/src/utils.c

void foobar()
   {
   }

#ifdef TESTING

// This code is if-defed away, is not compiled, but the substring "*/*" (in "*/*COMPLETE") 
// causes the simpler lexer that we have in ROSE for collecting macros and preprocessor 
// declarations to be fooled into thinking that the substring "#endif /* TESTING " is part 
// of a C style comment (that began with "*/*COMPLETE").  This may be outside of the 
// range of what I can fix with all the other pieces remaining to be fixed.
// This can be searched for using: grep -r "\*/\*COMPLETE" *

const char *
test_dir_matches_p()
   {

#error "DEAD CODE!"

     struct {
          char *dirlist[3];
          char *dir;
          bool result;
        } test_array[] = {
             { { "*/*COMPLETE", NULL, NULL }, "foo/!COMPLETE", true }
          };

  return NULL;
}

#endif /* TESTING */

