// Examples from: https://en.cppreference.com/w/cpp/20

#ifdef __has_include                           // Check if __has_include is present
#  if __has_include(<optional>)                // Check for a standard library
#    include <optional>
#  elif __has_include(<experimental/optional>) // Check for an experimental version
#    include <experimental/optional>
#  elif __has_include(<boost/optional.hpp>)    // Try with an external library
#    include <boost/optional.hpp>
#  else                                        // Not found at all
#     error "Missing <optional>"
#  endif
#endif
 
#ifdef __has_cpp_attribute                      // Check if __has_cpp_attribute is present
#  if __has_cpp_attribute(deprecated)           // Check for an attribute
#    define DEPRECATED(msg) [[deprecated(msg)]]
#  endif
#endif
#ifndef DEPRECATED
#    define DEPRECATED(msg)
#endif
 
DEPRECATED("foo() has been deprecated") void foo();
 
#if __cpp_constexpr >= 201304                   // Check for a specific version of a feature
#  define CONSTEXPR constexpr
#else
#  define CONSTEXPR inline
#endif
 
CONSTEXPR int bar(unsigned i)
{
#if __cpp_binary_literals                    // Check for the presence of a feature
    unsigned mask1 = 0b11000000;
    unsigned mask2 = 0b00000111;
#else
    unsigned mask1 = 0xC0;
    unsigned mask2 = 0x07;
#endif
    if ( i & mask1 )
        return 1;
    if ( i & mask2 )
        return 2;
    return 0;
}
 
int main()
{
}


