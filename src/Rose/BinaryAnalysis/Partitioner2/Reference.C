#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Partitioner2/Reference.h>

#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>

#include <SgAsmInstruction.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

Reference::~Reference() {}

Reference::Reference(const Function::Ptr &function)
    : function_(function), insn_(nullptr) {}

Reference::Reference(const Function::Ptr &function, const BasicBlock::Ptr &bblock, SgAsmInstruction *insn,
                     const Sawyer::Optional<rose_addr_t> &address)
    : function_(function), bblock_(bblock), insn_(insn), address_(address) {}

Reference::Reference(const BasicBlock::Ptr &bblock, SgAsmInstruction *insn, const Sawyer::Optional<rose_addr_t> &address)
    : bblock_(bblock), insn_(insn), address_(address) {}

Reference::Reference(SgAsmInstruction *insn, const Sawyer::Optional<rose_addr_t> &address)
    : insn_(insn), address_(address) {}

Reference::Reference(rose_addr_t address)
    : insn_(nullptr), address_(address) {}

Reference::Reference()
    : insn_(nullptr) {}

Reference::Granularity
Reference::granularity() const {
    if (address_)
        return ADDRESS;
    if (insn_)
        return INSTRUCTION;
    if (bblock_)
        return BASIC_BLOCK;
    if (function_)
        return FUNCTION;
    return EMPTY;
}

bool
Reference::isEmpty() const {
    return granularity() == EMPTY;
}

bool
Reference::hasFunction() const {
    return function_ != nullptr;
}

Function::Ptr
Reference::function() const {
    return function_;
}

bool
Reference::hasBasicBlock() const {
    return bblock_ != nullptr;
}

BasicBlock::Ptr
Reference::basicBlock() const {
    return bblock_;
}

bool
Reference::hasInstruction() const {
    return insn_ != nullptr;
}

SgAsmInstruction*
Reference::instruction() const {
    return insn_;
}

bool
Reference::hasExplicitAddress() const {
    return address_;
}

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
