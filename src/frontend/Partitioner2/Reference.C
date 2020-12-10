#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>
#include <Partitioner2/Reference.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

rose_addr_t
Reference::address() const {
    if (address_)
        return *address_;
    if (insn_)
        return insn_->get_address();
    if (bblock_)
        return bblock_->address();
    ASSERT_not_null2(function_, "empty references have no address");
    return function_->address();
}

bool
Reference::operator==(const Reference &other) const {
    Granularity g1 = granularity();
    Granularity g2 = other.granularity();
    return (g1==EMPTY && g2==EMPTY) || (g1==g2 && g1!=EMPTY && address()==other.address());
}

bool
Reference::operator<(const Reference &other) const {
    // Test for empty references.  The empty reference is less than all non-empty references.
    if (isEmpty() || other.isEmpty()) {
        if (isEmpty() && other.isEmpty())
            return false;
        if (isEmpty())
            return true;
        return false;
    }

    // All non-empty references have an address, so use that to distinguish them.
    if (address() != other.address())
        return address() < other.address();

    // Finer granularity is less than coarser granularity. I.e., a reference to just an address should appear earlier
    // in a sorted list than a reference to an instruction, etc.
    if (granularity() != other.granularity())
        return granularity() < other.granularity();

    // Same address and same granularity means they are equal
    return false;
}


} // namespace
} // namespace
} // namespace

#endif
