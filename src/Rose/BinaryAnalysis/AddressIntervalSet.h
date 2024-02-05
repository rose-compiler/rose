#ifndef ROSE_BinaryAnalysis_AddressIntervalSet_H
#define ROSE_BinaryAnalysis_AddressIntervalSet_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/AddressInterval.h>

#include <Sawyer/IntervalSet.h>

namespace Rose {
namespace BinaryAnalysis {

/** A set of virtual addresses. This is optimized for cases when many addresses are contiguous. */
using AddressIntervalSet = Sawyer::Container::IntervalSet<AddressInterval>;

} // namespace
} // namespace

std::ostream& operator<<(std::ostream&, const Rose::BinaryAnalysis::AddressIntervalSet&);

#endif
#endif
