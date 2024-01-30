#ifndef ROSE_UNUSED_H
#define ROSE_UNUSED_H

// Cause a variable to be used so that the compiler doesn't generate unused-parameter, unused-variable, or unused-but-set-variable
// warnings for that variable.  For instance given this code:
//
//    | 1|int foo(int x) {
//    | 2|#ifdef SOMETHING
//    | 3|    return x;
//    | 4|#else
//    | 5|    return 0;
//    | 6|#endif
//    | 7|}
//
// The compiler will emit an unused-parameter warning for 'x' at line 1. We can't just remove the formal argument name because
// it actually is used in certain configurations (just not this particular configuration).
//
//    | 1|int foo(int) {
//    | 2|#ifdef SOMETHING
//    | 3|    return x;     // error here
//    | 4|#else
//    | 5|    return 0;
//    | 6|#endif
//    | 7|}
//
//
// Another way is to conditionally specify the argument name as follows, but this is ugly (pretend that the argument list and body
// are much larger than shown here, and that we want the parens and braces to still match correctly for the sake of IDEs that use
// them in as syntax highlighting cues).
//
//    | 1|int foo(int
//    | 2|#ifdef SOMETHING
//    | 3|             x
//    | 4|#endif
//    | 5|       ) {
//    | 6|#ifdef SOMETHING
//    | 7|    return x;     // error here
//    | 8|#else
//    | 9|    return 0;
//    |10|#endif
//    |11|}
//
// The best way is to use C++11 attributes. Unfortunately this results in a different warning for older GCC compilers that don't
// understand the attribute:
//
//    | 1|int foo(int x [[maybe_unused]]) {   // possible "attribute directive ignored" warning
//    | 2|#ifdef SOMETHING
//    | 3|    return x;
//    | 4|#else
//    | 5|    return 0;
//    | 6|#endif
//    | 7|}
//
// Therefore, the only choice we have left is to actually use the variable in some way that doesn't result in another warning. Since
// every compiler is different, and for documentation purposes, we hide the special code in a macro like this:
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

#define ROSE_UNUSED(X) ((void)(X))

// Another possibility if the above doesn't work:
// #define ROSE_UNUSED(X) (*(volatile typeof(X)*)&(X) = (X))

#endif
