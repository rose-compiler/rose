#ifndef ROSE_PRAGMA_MESSAGE_H
#define ROSE_PRAGMA_MESSAGE_H

// Converts parameter X to a string after macro replacement
#define ROSE_STRINGIZE(X) ROSE_DO_STRINGIZE(X)
#define ROSE_DO_STRINGIZE(X) #X

// Possibly show a message from the compiler. Argument should be a string literal.
#if defined(__INTEL_COMPILER)
# define ROSE_PRAGMA_MESSAGE(x) __pragma(message(__FILE__, ":" ROSE_STRINGIZE(__LINE__) ": note: " x))
#elif defined(__GNUC__)
# define ROSE_PRAGMA_MESSAGE(x) _Pragma(ROSE_STRINGIZE(message(x)))
#elif defined(_MSC_VER)
# define ROSE_PRAGMA_MESSAGE(x) __pragma(message(__FILE__ ":" ROSE_STRINGIZE(__LINE__) ": note: " x))
#else
# define ROSE_PRAGMA_MESSAGE(x) /*silent*/
#endif

#endif
