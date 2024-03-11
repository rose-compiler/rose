#ifndef ROSE_BinaryAnalysis_AddressSet_H
#define ROSE_BinaryAnalysis_AddressSet_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Address.h>
#include <Sawyer/Set.h>

namespace Rose {
namespace BinaryAnalysis {

/** Set of addresses. */
using AddressSet = Sawyer::Container::Set<Address>;

} // namespace
} // namespace

#endif
#endif
