/*
Compiling the following code:
void get_mangled_name(){
#if 0
     '"';
#if 0
     '"';
#endif
#endif
}


with ROSE gives the following error:
rose_Cxx_Grammar.C:6:2: #endif without #if

Where rose_Cxx_Grammar.C is:
void get_mangled_name()
{
#if 0
#endif
#endif
}

This is probably caused by the lexer in the preprocessingmechanism
interpreting the two quotes in this source file as the beginning and end of a
string. If it does not recognize this as a string then it will process the
file just fine and recognize the second #if 0 as a preprocessing directive
and not linke now as part of a string.

Thanks
Andreas
*/

void get_mangled_name()
   {
#if 0
     '"';
#if 0
     '"';
#endif
#endif

// DQ (1/9/2009): Added test to make sure that this works properly 
// when the code is evaluated (not the original problem).
#if 1
     '"';
#if 1
     '"';
#endif
#endif
   }
