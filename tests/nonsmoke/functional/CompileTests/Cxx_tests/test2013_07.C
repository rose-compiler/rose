// Note that we need this to be defined so that we can avoid errors such as:
//    /usr/include/c++/4.3/x86_64-suse-linux/bits/gthr-default.h(100):
//    error: function "__gthrw_pthread_once" was referenced but not defined __gthrw(pthread_once)

#define _GLIBCXX__PTHREADS 1

// This is the shortest code that is a problem.
#include <string>

// This by itself compiles fine.
// #include <vector>
