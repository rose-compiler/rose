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

#if _MSC_VER
#define ROSE_ASSERT assert
#endif

// #warning "Is the assert macro seen?"

#ifndef ROSE_ASSERT
 #ifndef NDEBUG
  #define ROSE_ASSERT assert
 #else // We use assert(false) equivalents so often for "should not get here", but we don't want nontrivial side effects in asserts to be run when assert is disabled
  #define ROSE_ASSERT(exp) do {if (__builtin_constant_p(exp)) {if (exp) {} else (std::abort)();}} while (0)
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
// #ifdef __APPLE__
extern "C" 
{

 #ifdef USE_ROSE
   // DQ (9/3/2009): This is required for EDG to correctly compile
   // tps (01/22/2010) : gcc43 requires abort(void)
   void ROSE_ABORT() __THROW __attribute__ ((__noreturn__));
 #else
     // DQ (9/3/2009): This is required for Mac OSX to correctly compile
     #ifdef _MSC_VER
       // DQ (11/28/2009): This is a warning in MSVC ("warning C4273: 'abort' : inconsistent dll linkage")
       void ROSE_ABORT(void);
     #else
//#ifdef __GNUC__
//#  include <features.h>
//#if __GNUC_PREREQ(4,3)
//       If gcc_version >= 4.3
//#else
       void ROSE_ABORT() throw();
//#endif
//#endif
   #endif // MSC
 #endif // USE_ROSE
}

// throw rose_exception with user defined abort message
void ROSE_ABORT( const char *message );


#endif // ROSE_PROCESSSUPPORT_H
