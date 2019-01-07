// __has_include for C++17

// This appears to work for GNU g++ version 6.1, but fails for EDG 5.0 (or support there would need to be turned on).

#if __has_include(<optional>)
#  include <optional>
#  define have_optional 1
#elif __has_include(<experimental/optional>)
#  include <experimental/optional>
#  define have_optional 1
#  define experimental_optional 1
#else
#  define have_optional 0
#endif

