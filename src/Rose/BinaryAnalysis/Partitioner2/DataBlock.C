#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Partitioner2/DataBlock.h>

#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>
#include <Rose/BinaryAnalysis/Partitioner2/Utility.h>
#include <Rose/SourceLocation.h>

#include <SgAsmIntegerType.h>
#include <SgAsmVectorType.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

DataBlock::DataBlock()
    : isFrozen_(false), startVa_(0), type_(NULL) {}

DataBlock::DataBlock(Address startVa, SgAsmType *type)
    : isFrozen_(false), startVa_(startVa), type_(type) {}

DataBlock::~DataBlock() {}

// class method
DataBlock::Ptr
DataBlock::instance(Address startVa, SgAsmType *type) {
    ASSERT_not_null(type);
    return Ptr(new DataBlock(startVa, type));
}

// class method
DataBlock::Ptr
DataBlock::instanceBytes(Address startVa, size_t nBytes) {
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

const std::string&
DataBlock::comment() const {
    return comment_;
}

void
DataBlock::comment(const std::string &s) {
    comment_ = s;
}

const SourceLocation&
DataBlock::sourceLocation() const {
    return sourceLocation_;
}

void
DataBlock::sourceLocation(const SourceLocation &loc) {
    sourceLocation_ = loc;
}

AddressInterval
DataBlock::extent() const {
    return AddressInterval::baseSize(address(), size());
}

size_t
DataBlock::nAttachedOwners() const {
    return attachedBasicBlockOwners_.size() + attachedFunctionOwners_.size();
}

const std::vector<FunctionPtr>&
DataBlock::attachedFunctionOwners() const {
    return attachedFunctionOwners_;
}

const std::vector<BasicBlockPtr>&
DataBlock::attachedBasicBlockOwners() const {
    return attachedBasicBlockOwners_;
}

std::string
DataBlock::printableName() const {
    return "data block " + StringUtility::addrToString(address()) +
        (comment_.empty() ? "" : " \"" + StringUtility::cEscape(comment_) + "\"");
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

void
DataBlock::insertOwner(const BasicBlock::Ptr &bb) {
    ASSERT_not_null(bb);
    insertUnique(attachedBasicBlockOwners_, bb, sortBasicBlocksByAddress);
}

void
DataBlock::insertOwner(const Function::Ptr &function) {
    ASSERT_not_null(function);
    insertUnique(attachedFunctionOwners_, function, sortFunctionsByAddress);
}

void
DataBlock::eraseOwner(const BasicBlock::Ptr &bb) {
    ASSERT_not_null(bb);
    bool erased = eraseUnique(attachedBasicBlockOwners_, bb, sortBasicBlocksByAddress);
    ASSERT_always_require(erased);
}

void
DataBlock::eraseOwner(const Function::Ptr &function) {
    ASSERT_not_null(function);
    bool erased = eraseUnique(attachedFunctionOwners_, function, sortFunctionsByAddress);
    ASSERT_always_require(erased);
}

void
DataBlock::freeze() {
    isFrozen_ = true;
}

void
DataBlock::thaw() {
    isFrozen_ = false;
    ASSERT_require(attachedBasicBlockOwners_.empty());
    ASSERT_require(attachedFunctionOwners_.empty());
}

} // namespace
} // namespace
} // namespace

#endif
