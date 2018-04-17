// This specimen is crafted to test whether an escaped linefeed in a
// string literal messes up the placement of subsequent preprocessor
// directives.  The line spacing withint foo() is important.  The
// use of a struct is to prevent ROSE from bailing on an error before
// it generates the rose_*.c output.

struct S {
    int sm;
};

void foo(struct S *s){
"\
";
          if (1)
            {
#if 0
#endif
            }
          s->sm = 0;
}

