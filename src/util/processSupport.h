#ifndef ROSE_PROCESSSUPPORT_H
#define ROSE_PROCESSSUPPORT_H

#include <vector>
#include <string>
#include <cstdio>
#include <exception>
#include "rosedll.h"
#include <Sawyer/Assert.h>

// The various definistions of ROSE_ASSERT have been moved in to the
// following file. JFR 2020-Jun-05

#include "RoseAsserts.h"

ROSE_UTIL_API int systemFromVector(const std::vector<std::string>& argv);
FILE* popenReadFromVector(const std::vector<std::string>& argv);
// Assumes there is only one child process
int pcloseFromVector(FILE* f);

// Introducing class rose_excepction
// this class gets thrown by ROSE_ABORT
// it should probably inherit from std::runtime_error
// See also Rose::Diagnostics::FailedAssertion
class ROSE_UTIL_API rose_exception
    : public std::exception
{
    public:
        // call constructor with a reason for that exception
        explicit rose_exception( const char *what = "" );

        virtual const char *what() const throw();

    private:
        const char *what_;
};

// DQ (8/22/2009): Added throw since EDG wants to see that the throw options match when ROSE_ABORT is a macro to "abort()" in
// "stdlib.h".
// throws rose_exception with the reason "abort" void ROSE_ABORT();
//
#ifdef ROSE_ABORT
    // If ROSE_ABORT is #defined as abort then avoid redefining ::abort(). If ::abort() is redefined to throw an exception we
    // run the risk of infinite recursion since the C++ runtime's exception handler might itself call ::abort().  For example,
    // try adding "ROSE_ABORT()" to the beginning of CxxGrammarMetaProgram and see what happens! [Robb P. Matzke 2015-04-25]
#else
    extern "C" {
        #ifdef USE_ROSE
            // DQ (9/3/2009): This is required for EDG to correctly compile
            // tps (01/22/2010) : gcc43 requires abort(void)
            inline void ROSE_ABORT() __THROW __attribute__ ((__noreturn__));
        #elif defined(_MSC_VER)
            // DQ (11/28/2009): This is a warning in MSVC ("warning C4273: 'abort' : inconsistent dll linkage")
            inline ROSE_UTIL_API void ROSE_ABORT(void) { throw rose_exception("abort"); }
        #elif defined(__clang__)
            inline void ROSE_ABORT(void) { throw rose_exception("abort"); }
        #else
            inline void ROSE_ABORT() throw() { throw rose_exception("abort"); }
        #endif
    }
#endif

// throw rose_exception with user defined abort message
ROSE_UTIL_API void ROSE_ABORT(const char *message);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Assertion handling
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Rose {

/** Aborts for a failed assertion. */
ROSE_UTIL_API void abortOnFailedAssertion(const char*, const char*, const std::string&, const char*, unsigned, const char*);

/** Exits with non-zero status for a failed assertion. */
ROSE_UTIL_API void exitOnFailedAssertion(const char*, const char*, const std::string&, const char*, unsigned, const char*);

/** Throws an exception for a failed assertion.
 *
 *  Throws @ref FailedAssertion exception. */
ROSE_UTIL_API void throwOnFailedAssertion(const char*, const char*, const std::string&, const char*, unsigned, const char*);

/** Property: behavior of failed assertions.
 *
 *  The property value can be @ref abortOnFailedAssertion, @ref exitOnFailedAsssertion, or @ref throwOnFailedAssertion.
 *
 *  Setting the behavior to null will cause a default behavior to be used. The default behavior is chosen based on the ROSE
 *  configuration.  The retrieved property will be null only if the property has never been set and the ROSE library has never
 *  been initialized; in this case, Sawyer::Assert macros abort for failed assertions.
 *
 * @{ */
ROSE_UTIL_API void failedAssertionBehavior(Sawyer::Assert::AssertFailureHandler handler);
Sawyer::Assert::AssertFailureHandler failedAssertionBehavior();
/** @} */

/** Exception that can be thrown for a failed assertion.
 *
 *  The <code>std::runtime_error::what</code> string will be the @c expr if non-null, otherwise the @c mesg. */
struct FailedAssertion: std::runtime_error {
    const char *mesg;                                   // a short message, like "assertion failed", "not implemented" etc.
    const char *expr;                                   // C++ expression that caused the failure (optional)
    std::string note;                                   // second argument from ASSERT_*() macros, or empty
    const char *fileName;                               // name of file where assertion failed
    unsigned lineNumber;                                // line number where assertion failed (1-origin)
    const char *functionName;                           // function name (perhaps with arg types) where assertion failed
    FailedAssertion(const char *mesg, const char *expr, const std::string &note,
                    const char *fileName, unsigned lineNumber, const char *functionName)
        : std::runtime_error(expr?expr:mesg), mesg(mesg), expr(expr), note(note), fileName(fileName),
          lineNumber(lineNumber), functionName(functionName) {}
    ~FailedAssertion() throw () {};
};

} // namespace

#endif // ROSE_PROCESSSUPPORT_H
