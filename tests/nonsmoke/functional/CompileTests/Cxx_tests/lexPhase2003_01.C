// Example which demonstrates a previous bug in lex phase of extracting comments and CPP directives.
// The problem was that there were too many braces and this confused the stack mechanism which
// counted the braces (this mechanism is now only turned on when parsing the extern "C" { }
// declarations. Thus the bug can be recreated with this construct, and example is shown in 
// the test code lexPhase2003_02.C

class X
   {
#if 0
     int z;
   };
#else
     int a;
   };
#endif

void foo ()
   {
#if 0
     int z;
};

#else
     int a;
};

#endif

