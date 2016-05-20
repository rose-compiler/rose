#ifndef ROSE_PROCESSSUPPORT_H
#define ROSE_PROCESSSUPPORT_H

#include <vector>
#include <string>
#include <cstdio>
#include <exception>
#include "rosedll.h"
#include <Sawyer/Assert.h>

#if 0
// I think that this may already be used.
// DQ (4/23/2016): Added to inlcude ROSE_ASSERTION_EXIT and related macro definitions.
#include "Diagnostics.h"

// Check that ROSE_ASSERTION_BEHAVIOR is defined.
#ifndef ROSE_ASSERTION_BEHAVIOR

#ifndef ROSE_ASSERTION_EXIT
// #warning "We want to have ROSE_ASSERTION_EXIT be defined"
#endif

// This determines how failed assertions should behave, we want it to be defined as: 
#define ROSE_ASSERTION_BEHAVIOR ROSE_ASSERTION_EXIT

// #error "We want to have ROSE_ASSERTION_BEHAVIOR be defined"
#endif
#endif

ROSE_UTIL_API int systemFromVector(const std::vector<std::string>& argv);
FILE* popenReadFromVector(const std::vector<std::string>& argv);
// Assumes there is only one child process
int pcloseFromVector(FILE* f);

// Logic assertion checking.
//
// ROSE_ASSERT can be used to check assertions in program logic; it should not be used to report errors caused by users. This
// macro is used in approximately 36,000 places in the library, tutorial, tests, and projects and is sometimes used
// (incorrectly) to report user errors. Therefore it is impracticable to inspect all uses, and we must continue to ensure that
// all calls to ROSE_ASSERT are checked at runtime regardless of whether NDEBUG is defined.
//
// New code should use Sawyer's ASSERT_* macros to check logic assertions because:
//    + It has two-argument versions where the second argument is the string describing what's wrong in more user-friendly
//      terms. E.g. ASSERT_forbid2(s.empty(), "symbol name cannot be the empty string").
//    + The name encodes whether the macro is disabled when NDEBUG is defined. The "always" versions are never disabled, nor
//      are the macros ASSERT_not_implemented, ASSERT_not_reachable, TODO, and FIXME. E.g., ASSERT_always_require(...).
//    + Failure behavior is configurable via rose::Diagnostics API, frontend() command-line, or autotools/cmake.  E.g.,
//      a failing assertion can abort, exit with non-zero status, or throw a rose::Diagnostics::FailedAssertion exception.
//    + Output from failed assertions is easier to read because it's split across multiple lines. On ANSI terminals it will
//      show up in bright red.
//
// Additional documentation can be found here:
//   + Sawyer ASSERT_* macros (https://hoosierfocus.com/~matzke/Sawyer/namespaceSawyer_1_1Assert.html)
//   + rose::Diagnostics (http://rosecompiler.org/ROSE_HTML_Reference/namespacerose_1_1Diagnostics.html)
#ifndef ROSE_ASSERT
    #if _MSC_VER
        #include <assert.h>
        #define ROSE_ASSERT assert
    #elif defined(ROSE_ASSERTION_BEHAVIOR)
        // Use Sawyer ASSERT_require because it supports different behaviors (abort, exit, or throw) based on ROSE
        // configuration and overridden at runtime by frontend() command-line switches or the rose::Diagnostics API. However,
        // since "ROSE_ASSERT(false)" and equivalents are used so often for "should not get here", we must ensure that
        // ROSE_ASSERT is checked regardless of whether NDEBUG is defined (thus the "_always_" version).  Also, the Sawyer
        // ASSERT_always_* macros evaluate their arguments exactly once. New code should use ASSERT_not_implemented,
        // ASSERT_not_reachable, TODO, or FIXME for "should not get here".
        #define ROSE_ASSERT ASSERT_always_require
    #elif !defined(NDEBUG)
        // This is the original pre-Sawyer version when NDEBUG is not defined.
        #define ROSE_ASSERT assert
    #else
        // This is the original pre-Sawyer version when NDEBUG is defined. It came with this comment: We use "assert(false)"
        // equivalents so often for "should not get here", but we don't want nontrivial side effects in asserts to be run when
        // assert is disabled.
        #define ROSE_ASSERT(exp) do {if (__builtin_constant_p(exp)) {if (exp) {} else (std::abort)();}} while (0)
    #endif
#endif
 


// introducing class rose_excepction
// this class gets thrown by ROSE_ABORT
// it should probably inherit from std::runtime_error
// See also rose::Diagnostics::FailedAssertion
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
namespace rose {

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
