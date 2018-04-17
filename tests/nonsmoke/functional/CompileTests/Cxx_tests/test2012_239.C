extern "C" int exit(int code);

#if 1
// Note that this will unparse with two nested levels of extern "C" in use.
//    1) collected from the gathering of comments and CPP directives
//    2) generated as part of the extern "C" specification on each declaration.
//    3) Not clear which one I want to keep, but both appear to be fine for now.
extern "C" {
  int printf(char const *fmt, ...);
  int rand();
}
#endif
