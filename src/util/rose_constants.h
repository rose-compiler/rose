// This file defines constants, flags, etc. that are so useful that they're at the very top
// "Rose" namespace, and included in all user code via #include <rose.h>.
//
// Do not put #define constants in this file unless they follow the proper ROSE naming scheme.
//
#ifndef ROSE_constants_H
#define ROSE_constants_H

#include <rosedll.h>
#include <cstddef>

namespace Rose {

/** Effictively unlimited size.
 *
 *  This is used, among other places, as a default argument for functions that limit the size of their return value in some
 *  way. Use this instead of "(size_t)(-1)" for better self-documenting code. */
const size_t UNLIMITED(-1);

/** Invalid array index.
 *
 *  This is used to represent lack of a valid array index. Use this instead of "(size_t)(-1)" for better self-documenting
 *  code. */
const size_t INVALID_INDEX(-1);

} // namespace
#endif
