#ifndef ROSE_BinaryAnalysis_Address_H
#define ROSE_BinaryAnalysis_Address_H
#include <RoseFirst.h>

#include <cstdint>

namespace Rose {
namespace BinaryAnalysis {

/** Address. */
using Address = std::uint64_t;

} // namespace
} // namespace

// Backward compatible name used from 2008-2024 at the root namespace.
using rose_addr_t = Rose::BinaryAnalysis::Address;

#endif
