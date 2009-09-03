#ifndef ROSE_PROCESSSUPPORT_H
#define ROSE_PROCESSSUPPORT_H

#include <vector>
#include <string>
#include <cstdio>
#include <exception>

int systemFromVector(const std::vector<std::string>& argv);
FILE* popenReadFromVector(const std::vector<std::string>& argv);
// Assumes there is only one child process
int pcloseFromVector(FILE* f);

#ifndef ROSE_ASSERT
 #ifndef NDEBUG
  #define ROSE_ASSERT assert
 #else // We use assert(false) equivalents so often for "should not get here", but we don't want nontrivial side effects in asserts to be run when assert is disabled
  #define ROSE_ASSERT(x) do {if (__builtin_constant_p(x)) {if (x) {} else (std::abort)();}} while (0)
 #endif
#endif

// introducing class rose_excepction
// this class gets thrown by ROSE_ABORT
class rose_exception
    : public std::exception
{
    public:
        // call constructor with a reason for that exception
        explicit rose_exception( const char *what = "" );

        virtual const char *what() const throw();

    private:
        const char *what_;
};

// define ROSE_ABORT as a function instead of a macro
// this allows us overloading

// DQ (8/22/2009): Added throw since EDG wants to see that the throw options match when ROSE_ABORT is a macro to "abort()" in "stdlib.h".
// throws rose_exception with the reason "abort"
// void ROSE_ABORT();
#ifdef USE_ROSE
// DQ (9/3/2009): This is required for EDG to correctly compile
void ROSE_ABORT() __THROW __attribute__ ((__noreturn__));
#else
// DQ (9/3/2009): This is required for Mac OSX to correctly compile
void ROSE_ABORT();
#endif

// throw rose_exception with user defined abort message
void ROSE_ABORT( const char *message );


#endif // ROSE_PROCESSSUPPORT_H
