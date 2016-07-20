#include <bits/c++config.h>
#include <bits/c++config.h>
#include <locale.h>
#include <stddef.h>

// DQ (2/9/2014): This macro does not exist in later versions of the GNU g++ header files.
// #if !( (__GNUC__ == 4) && (__GNUC_MINOR__ >= 6) )
// #if ( defined(__clang__) == 0 && !( (__GNUC__ == 4) && (__GNUC_MINOR__ >= 6) ))
#if ( defined(__clang__) == 0 && !( (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6) )))
_GLIBCXX_BEGIN_NAMESPACE(std)
 using ::ptrdiff_t;
 using ::size_t;
_GLIBCXX_END_NAMESPACE
#endif

#define _GLIBCXX_NUM_CATEGORIES 6

extern "C" __typeof(uselocale) __uselocale;

