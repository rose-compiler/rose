#ifndef ROSE_BinaryAnalysis_AddressInterval_H
#define ROSE_BinaryAnalysis_AddressInterval_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Sawyer/Interval.h>

namespace Rose {
namespace BinaryAnalysis {

/** An interval of addresses. */
using AddressInterval = Sawyer::Container::Interval<rose_addr_t>;

} // namespace
} // namespace

std::ostream& operator<<(std::ostream&, const Rose::BinaryAnalysis::AddressInterval&);

#endif
#endif
