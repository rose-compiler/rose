#ifndef ROSE_DEPRECATED_H
#define ROSE_DEPRECATED_H

// The ROSE_DEPRECATED marker unconditionally marks a function or variable as deprecated and will produce a warning if
// whenever a use of that function or variable occurs.  Do not disable this macro; see ROSE_DEPRECATED_FUNCTION instead.
// If you mark a function or variable as deprecated, then BE SURE TO FIX PLACES WHERE IT IS USED IN ROSE!!!  The WHY argument
// should be a string literal (unevaluated) describing why it's deprecated or what to use instead.
#if defined(__clang__)
# define ROSE_DEPRECATED(WHY) __attribute__((deprecated(WHY)))
#elif defined(__GNUC__)
#   define ROSE_DEPRECATED(WHY) __attribute__((deprecated))
#elif defined(_MSC_VER)
#   define ROSE_DEPRECATED(WHY) /*deprecated*/
#else
#   define ROSE_DEPRECATED(WHY) /*deprecated*/
#endif

// The ROSE_DEPRECATED_FUNCTION and ROSE_DEPRECATED_VARIABLE conditionally mark a function or variable as deprecated.  At this
// time, ROSE itself contains hundreds of uses of deprecated functions and variables because the people that marked those
// functions and variables as deprecated did not also fix ROSE to avoid calling them.  The warnings can be suppressed by
// defining ROSE_SUPPRESS_DEPRECATION_WARNINGS on the compiler command-line during configuration.
//
// For the time being we explicitly define ROSE_SUPPRESS_DEPRECATION_WARNINGS because of the problem mentioned above. ROSE
// authors should only add ROSE_DEPRECATED markers and not use new ROSE_DEPRECATED_FUNCTION or ROSE_DEPRECATED_VARAIBLE since
// the latter two do nothing. AND BE SURE TO FIX PLACES IN ROSE WHERE THE DEPRECATED THING IS USED!!!!!
#undef ROSE_SUPPRESS_DEPRECATION_WARNINGS
#define ROSE_SUPPRESS_DEPRECATION_WARNINGS
#if !defined(ROSE_SUPPRESS_DEPRECATION_WARNINGS)
#   if !defined(ROSE_DEPRECATED_FUNCTION)
#       define ROSE_DEPRECATED_FUNCTION ROSE_DEPRECATED
#   endif
#   if !defined(ROSE_DEPRECATED_VARIABLE)
#       define ROSE_DEPRECATED_VARIABLE ROSE_DEPRECATED
#   endif
#else
#   if !defined(ROSE_DEPRECATED_FUNCTION)
#       define ROSE_DEPRECATED_FUNCTION /*deprecated*/
#   endif
#   if !defined(ROSE_DEPRECATED_VARIABLE)
#       define ROSE_DEPRECATED_VARIABLE /*deprecated*/
#   endif
#endif

// Used to mark deprecated functions and advertise that fact to developers and especially to end users.  This is sometimes
// turned off during development (because it's annoying) by defining ROSE_SUPPRESS_DEPRECATION_WARNINGS when configuring.
#if !defined(ROSE_SUPPRESS_DEPRECATION_WARNINGS)
#   if defined(__GNUC__)
        // Put ROSE_DEPRECATED_FUNCTION after the declaration, i.e.: int Foo::bar() const ROSE_DEPRECATED_FUNCTION;
#       define ROSE_DEPRECATED_FUNCTION __attribute__((deprecated))
#       define ROSE_DEPRECATED_VARIABLE __attribute__((deprecated))
#   elif defined(_MSC_VER)
        // Microsoft Visual C++ needs "__declspec(deprecated)" before the declaration. We don't really want to put
        // ROSE_DEPRECATED_FUNCTION both before and after functions, so we just don't worry about advertising deprecation when
        // using Microsoft compilers.  Use MinGW instead if you want a real C++ compiler on Windows.
#       define ROSE_DEPRECATED_FUNCTION /*deprecated*/
#       define ROSE_DEPRECATED_VARIABLE /*deprecated*/
#   else
        // No portable way to mark C++ functions as deprecated.
#       define ROSE_DEPRECATED_FUNCTION /*deprecated*/
#       define ROSE_DEPRECATED_VARIABLE /*deprecated*/
#   endif
#else
#   define ROSE_DEPRECATED_FUNCTION /*deprecated*/
#   define ROSE_DEPRECATED_VARIABLE /*deprecated*/
#endif

#endif
