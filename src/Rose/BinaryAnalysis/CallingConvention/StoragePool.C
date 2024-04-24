#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/CallingConvention/StoragePool.h>

#include <Rose/Affirm.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>

#include <SgAsmType.h>
#include <SgAsmFloatType.h>
#include <SgAsmIntegerType.h>
#include <SgAsmPointerType.h>
#include <Cxx_GrammarDowncast.h>

#include <algorithm>
#include <string>
#include <vector>

namespace Rose {
namespace BinaryAnalysis {
namespace CallingConvention {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TypePredicate
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TypePredicate::~TypePredicate() {}

TypePredicate::TypePredicate() {}

bool
TypePredicate::operator()(SgAsmType *type) const {
    return test(type);
}

// C++14 workaround:
//   use        `newTypePredicateFunctor(x)`
//   instead of `new TypePredicateFunctor(x)`
template<class F>
static TypePredicateFunctor<F>*
newTypePredicateFunctor(F f) {
    return new TypePredicateFunctor<F>(f);
}

TypePredicate::Ptr
isAnyType() {
    return TypePredicate::Ptr(newTypePredicateFunctor([](SgAsmType*) {
        return true;
    }));
}

TypePredicate::Ptr
isNonFpNotWiderThan(size_t nBits) {
    return TypePredicate::Ptr(newTypePredicateFunctor([nBits](SgAsmType *type) {
        ASSERT_not_null(type);
        return !isSgAsmFloatType(type) && type->get_nBits() <= nBits;
    }));
}

TypePredicate::Ptr
isFpNotWiderThan(size_t nBits) {
    return TypePredicate::Ptr(newTypePredicateFunctor([nBits](SgAsmType *type) {
        ASSERT_not_null(type);
        return isSgAsmFloatType(type) && type->get_nBits() <= nBits;
    }));
}

TypePredicate::Ptr
isIntegerNotWiderThan(size_t nBits) {
    return TypePredicate::Ptr(newTypePredicateFunctor([nBits](SgAsmType *type) {
        ASSERT_not_null(type);
        return isSgAsmIntegerType(type) && type->get_nBits() <= nBits;
    }));
}

TypePredicate::Ptr
isPointerNotWiderThan(size_t nBits) {
    return TypePredicate::Ptr(newTypePredicateFunctor([nBits](SgAsmType *type) {
        ASSERT_not_null(type);
        return isSgAsmPointerType(type) && type->get_nBits() <= nBits;
    }));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// StoragePoolBase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

StoragePoolBase::~StoragePoolBase() {}

StoragePoolBase::StoragePoolBase(const std::string &name, const TypePredicate::ConstPtr &canStore)
    : name_(name), canStore_(canStore) {
    ASSERT_not_null(canStore);
}

bool
StoragePoolBase::canStore(SgAsmType *type) const {
    ASSERT_not_null(canStore_);
    ASSERT_not_null(type);
    return canStore_->test(type);
}

std::vector<ConcreteLocation>
StoragePoolBase::constantLocations() const {
    return {};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// StoragePoolEnumerated
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

StoragePoolEnumerated::~StoragePoolEnumerated() {}

StoragePoolEnumerated::StoragePoolEnumerated(const std::string &name, const TypePredicate::ConstPtr &canStore)
    : StoragePoolBase(name, canStore) {}

StoragePoolEnumerated::Ptr
StoragePoolEnumerated::instance(const std::string &name, const TypePredicate::ConstPtr &canStore) {
    return Ptr(new StoragePoolEnumerated(name, canStore));
}

void
StoragePoolEnumerated::append(const ConcreteLocation &loc) {
    locations_.push_back(loc);
}

StoragePoolBase::Ptr
StoragePoolEnumerated::copy() const {
    return Ptr(new StoragePoolEnumerated(*this));
}

std::vector<ConcreteLocation>
StoragePoolEnumerated::constantLocations() const {
    std::vector<ConcreteLocation> retval;
    std::copy_if(locations_.begin(), locations_.end(), std::back_inserter(retval),
                 [](const ConcreteLocation &loc) {
                     return loc.type() == ConcreteLocation::REGISTER || loc.type() == ConcreteLocation::ABSOLUTE;
                 });
    return retval;
}

std::vector<ConcreteLocation>
StoragePoolEnumerated::consume(SgAsmType *type) {
    if (current_ < locations_.size() && canStore(type)) {
        return {locations_[current_++]};
    } else {
        return {};
    }
}

void
StoragePoolEnumerated::reset() {
    current_ = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// StoragePoolStack
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

StoragePoolStack::~StoragePoolStack() {}

StoragePoolStack::StoragePoolStack(const std::string &name, const TypePredicate::ConstPtr &canStore,
                                   const Architecture::Base::ConstPtr &arch)
    : StoragePoolBase(name, canStore), valuePadding_(1, notnull(arch)->bitsPerWord()),
      alignment_(1, arch->bitsPerWord()),
      baseRegister_(notnull(arch->registerDictionary())->stackPointerRegister()) {}

StoragePoolStack::Ptr
StoragePoolStack::instance(const std::string &name, const TypePredicate::ConstPtr &canStore,
                           const Architecture::Base::ConstPtr &arch) {
    return Ptr(new StoragePoolStack(name, canStore, arch));
}

StoragePoolBase::Ptr
StoragePoolStack::copy() const {
    return Ptr(new StoragePoolStack(*this));
}

StackDirection
StoragePoolStack::stackDirection() const {
    return stackDirection_;
}

void
StoragePoolStack::stackDirection(StackDirection x) {
    stackDirection_ = x;
}

int64_t
StoragePoolStack::initialOffset() const {
    return initialOffset_;
}

void
StoragePoolStack::initialOffset(int64_t x) {
    initialOffset_ = x;
}

size_t
StoragePoolStack::minimumValueSize() const {
    return minimumValueSize_;
}

void
StoragePoolStack::minimumValueSize(size_t x) {
    minimumValueSize_ = x;
}

const Alignment&
StoragePoolStack::valuePadding() const {
    return valuePadding_;
}

void
StoragePoolStack::valuePadding(const Alignment &x) {
    valuePadding_ = x;
}

ByteOrder::Endianness
StoragePoolStack::valueJustification() const {
    return valueJustification_;
}

void
StoragePoolStack::valueJustification(ByteOrder::Endianness x) {
    valueJustification_ = x;
}

const Alignment&
StoragePoolStack::alignment() const {
    return alignment_;
}

void
StoragePoolStack::alignment(const Alignment &x) {
    ASSERT_require(x.nBits() > 0);
    alignment_ = x;
}

RegisterDescriptor
StoragePoolStack::baseRegister() const {
    return baseRegister_;
}

void
StoragePoolStack::baseRegister(RegisterDescriptor x) {
    baseRegister_ = x;
}

std::vector<ConcreteLocation>
StoragePoolStack::consume(SgAsmType *type) {
    ASSERT_not_null(type);
    std::vector<ConcreteLocation> retval;
    const size_t typeBytes = (type->get_nBits() + 7) / 8;
    if (const auto needBytes = valuePadding_.alignUp(std::max(minimumValueSize_, typeBytes))) {
        if (StackDirection::GROWS_DOWN == stackDirection_) {
            // Since the stack grows down, the arguments that are already pushed are *above* the stack pointer.
            ASSERT_require(offset_ >= *needBytes);
            offset_ = *alignment_.alignUp(offset_);
            retval.push_back(ConcreteLocation(baseRegister_, offset_));
            offset_ += *needBytes;
        } else {
            // Since the stack grows down, the arguments that are already pushed are *below* the stack pointer.
            ASSERT_require(StackDirection::GROWS_UP == stackDirection_);
            offset_ = alignment_.alignDown(offset_ - *needBytes);
            retval.push_back(ConcreteLocation(baseRegister_, offset_));
        }
    }
    return retval;
}

void
StoragePoolStack::reset() {
    offset_ = initialOffset_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Allocator
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Allocator::~Allocator() {}

Allocator::Allocator() {}

Allocator::Ptr
Allocator::instance() {
    return Ptr(new Allocator);
}

void
Allocator::append(const StoragePoolBase::ConstPtr &pool) {
    ASSERT_not_null(pool);
    initialPools_.push_back(pool);
}

void
Allocator::reset() {
    activePools_.clear();
    for (const auto &pool: initialPools_) {
        activePools_.push_back(pool->copy());
        activePools_.back()->reset();
    }
}

std::vector<ConcreteLocation>
Allocator::allocate(SgAsmType *type) {
    for (const auto &pool: activePools_) {
        auto found = pool->consume(type);
        if (!found.empty())
            return found;
    }
    return {};
}

} // namespace
} // namespace
} // namespace

#endif
