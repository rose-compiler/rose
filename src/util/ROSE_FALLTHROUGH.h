#ifndef ROSE_FALLTHROUGH_H
#define ROSE_FALLTHROUGH_H

// Marks case branches that fall through to the next case, so that the compiler does not generate
// "this statement may fall through" warnings (i.e., GCC).
//
//    | 1|int foo(int x) {
//    | 2|  int res = 0;
//    | 3|  switch(x) {
//    | 4|    case 1: ++res;
//    | 5|    default: ++res; break;
//    | 6|  }
//    | 7|  return res;
//    | 8|}
//
// The compiler will emit a implicit-fallthrough warning for the end of 'case 0:' at line 4.
//
// The best way is to use C++17 attributes.
//
//    | 1|int foo(int x) {
//    | 2|  int res = 0;
//    | 3|  switch(x) {
//    | 4|    case 1: ++res; [[fallthrough]];
//    | 5|    default: ++res; break;
//    | 6|  }
//    | 7|  return res;
//    | 8|}
//
// Until we support C++17 in ROSE fully, we can rely on older attributes for the GNU compiler.
// [[gnu::fallthrough]];
//
//    | 1|int foo(int x) {
//    | 2|#ifdef SOMETHING
//    | 3|    return x;
//    | 4|#else
//    | 5|    ROSE_UNUSED(x);
//    | 6|    return 0;
//    | 7|#endif
//    | 8|}
//
// reference for gcc: https://developers.redhat.com/blog/2017/03/10/wimplicit-fallthrough-in-gcc-7


#if __cplusplus >= 201703L
#define ROSE_FALLTHROUGH [[fallthrough]]
#elif defined __GNUC__
#define ROSE_FALLTHROUGH [[gnu::fallthrough]]
#else
// this may not work as intended
#define ROSE_FALLTHROUGH /* FALL THROUGH */
#endif


#endif
