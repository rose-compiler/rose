#ifndef ROSE_ASSERTS_H
#define ROSE_ASSERTS_H

/*------------------------------------------------------------------------------
|
|  Note that much of this file was extracted from src/util/processSupport.h
|  JFR 2020-Jun-05
|
+-----------------------------------------------------------------------------*/

#include "rosedll.h"
#include <Sawyer/Assert.h>

/*{---------------------------------------------------------------------------*/
#if 0
/* I think that this may already be used. */
/* DQ (4/23/2016): Added to include ROSE_ASSERTION_EXIT and related macro definitions. */
#  include "Diagnostics.h"

/* Check that ROSE_ASSERTION_BEHAVIOR is defined. */
#  ifndef ROSE_ASSERTION_BEHAVIOR

#    ifndef ROSE_ASSERTION_EXIT
#    endif

/* This determines how failed assertions should behave, we want it to be defined as: */
#    define ROSE_ASSERTION_BEHAVIOR ROSE_ASSERTION_EXIT

/* # error "We want to have ROSE_ASSERTION_BEHAVIOR be defined" */
#  endif
#endif
/*}---------------------------------------------------------------------------*/

/*{-----------------------------------------------------------------------------
|
|  Logic assertion checking.
|
|  ROSE_ASSERT can be used to check assertions in program logic; it should not be used to report errors caused by users. This
|  macro is used in approximately 36,000 places in the library, tutorial, tests, and projects and is sometimes used
|  (incorrectly) to report user errors. Therefore it is impracticable to inspect all uses, and we must continue to ensure that
|  all calls to ROSE_ASSERT are checked at runtime regardless of whether NDEBUG is defined.
|
|  New code should use Sawyer's ASSERT_* macros to check logic assertions because:
|     + It has two-argument versions where the second argument is the string describing what's wrong in more user-friendly
|       terms. E.g. ASSERT_forbid2(s.empty(), "symbol name cannot be the empty string").
|     + The name encodes whether the macro is disabled when NDEBUG is defined. The "always" versions are never disabled, nor
|       are the macros ASSERT_not_implemented, ASSERT_not_reachable, TODO, and FIXME. E.g., ASSERT_always_require(...).
|     + Failure behavior is configurable via Rose::Diagnostics API, frontend() command-line, or autotools/cmake.  E.g.,
|       a failing assertion can abort, exit with non-zero status, or throw a Rose::Diagnostics::FailedAssertion exception.
|     + Output from failed assertions is easier to read because it's split across multiple lines. On ANSI terminals it will
|       show up in bright red.
|
|  Additional documentation can be found here:
|    + Sawyer ASSERT_* macros (https://hoosierfocus.com/~matzke/Sawyer/namespaceSawyer_1_1Assert.html)
|    + Rose::Diagnostics (http://rosecompiler.org/ROSE_HTML_Reference/namespacerose_1_1Diagnostics.html)
|
+-----------------------------------------------------------------------------*/

#ifndef ROSE_ASSERT
    #if _MSC_VER
        #include <assert.h>
        #define ROSE_ASSERT assert
    #elif defined(ROSE_ASSERTION_BEHAVIOR)
        /* Use Sawyer ASSERT_require because it supports different behaviors (abort, exit, or throw) based on ROSE
        |  configuration and overridden at runtime by frontend() command-line switches or the Rose::Diagnostics API. However,
        |  since "ROSE_ASSERT(false)" and equivalents are used so often for "should not get here", we must ensure that
        |  ROSE_ASSERT is checked regardless of whether NDEBUG is defined (thus the "_always_" version).  Also, the Sawyer
        |  ASSERT_always_* macros evaluate their arguments exactly once. New code should use ASSERT_not_implemented,
        |  ASSERT_not_reachable, TODO, or FIXME for "should not get here".
        */
        #define ROSE_ASSERT ASSERT_always_require
    #elif !defined(NDEBUG)
        /* This is the original pre-Sawyer version when NDEBUG is not defined.*/
        #define ROSE_ASSERT assert
    #else
        /* This is the original pre-Sawyer version when NDEBUG is defined. It came with this comment: We use "assert(false)"
        |  equivalents so often for "should not get here", but we don't want nontrivial side effects in asserts to be run when
        |  assert is disabled.
        */
        #define ROSE_ASSERT(exp) do {if (__builtin_constant_p(exp)) {if (exp) {} else (std::abort)();}} while (0)
    #endif
#endif

/*}---------------------------------------------------------------------------*/

#endif

