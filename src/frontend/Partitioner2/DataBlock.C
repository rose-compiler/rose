#include "sage3basic.h"
#include <Partitioner2/DataBlock.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

// class method
DataBlock::Ptr
DataBlock::instance(rose_addr_t startVa, SgAsmType *type) {
    ASSERT_not_null(type);
    return Ptr(new DataBlock(startVa, type));
}

// class method
DataBlock::Ptr
DataBlock::instanceBytes(rose_addr_t startVa, size_t nBytes) {
    SgAsmType *type = SageBuilderAsm::buildTypeVector(nBytes, SageBuilderAsm::buildTypeU8());
    return instance(startVa, type);
}

size_t
DataBlock::size() const {
    ASSERT_not_null(type_);
    return type_->get_nBytes();
}

void
DataBlock::type(SgAsmType *t) {
    ASSERT_not_null(t);
    ASSERT_forbid(isFrozen_);
    type_ = t;
}

AddressInterval
DataBlock::extent() const {
    return AddressInterval::baseSize(address(), size());
}

size_t
DataBlock::incrementOwnerCount() {
    return ++nAttachedOwners_;
}

size_t
DataBlock::decrementOwnerCount() {
    ASSERT_require(nAttachedOwners_ > 0);
    return --nAttachedOwners_;
}

void
DataBlock::nAttachedOwners(size_t n) {
    nAttachedOwners_ = n;
}

std::string
DataBlock::printableName() const {
    return "data block " + StringUtility::addrToString(address());
}

std::vector<uint8_t>
DataBlock::read(const MemoryMap::Ptr &map) const {
    std::vector<uint8_t> retval(size());
    if (map) {
        size_t nread = map->at(address()).read(retval).size();
        retval.resize(nread);
    }
    return retval;
}

} // namespace
} // namespace
} // namespace
