// This file defines constants, flags, etc. that are so useful that they're at the very top
// "Rose" namespace, and included in all user code via #include <rose.h>.
//
// Do not put #define constants in this file unless they follow the proper ROSE naming scheme.
//
#ifndef ROSE_Constants_H
#define ROSE_Constants_H
#include <RoseFirst.h>

#include <rosedll.h>
#include <cstddef>

namespace Rose {

/** Effectively unlimited size.
 *
 *  This is used, among other places, as a default argument for functions that limit the size of their return value in some
 *  way. Use this instead of "(size_t)(-1)" for better self-documenting code. */
const size_t UNLIMITED{static_cast<size_t>(-1)};

/** Invalid array index.
 *
 *  This is used to represent lack of a valid array index. Use this instead of "(size_t)(-1)" for better self-documenting
 *  code. */
const size_t INVALID_INDEX{static_cast<size_t>(-1)};

/** Width of the name field when dumping binary analysis AST nodes.
 *
 *  This is the minimum width in characters for the name field when dumping AST node information in human-readable, tabular
 *  format. This format is used by many of the binary analysis nodes in their `dump` functions. */
const size_t DUMP_FIELD_WIDTH{64};

} // namespace
#endif
