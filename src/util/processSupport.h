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

#include <ROSE_ABORT.h>
#include <ROSE_ASSERT.h>

ROSE_UTIL_API int systemFromVector(const std::vector<std::string>& argv);
FILE* popenReadFromVector(const std::vector<std::string>& argv);
// Assumes there is only one child process
int pcloseFromVector(FILE* f);

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
