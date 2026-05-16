#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryState.h>

#include <Rose/As.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Exception.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Merger.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SValue.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace BaseSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MemoryState
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MemoryState::MemoryState()
    : AddressSpace(AddressSpace::Purpose::MEMORY, "memory"), byteOrder_(ByteOrder::ORDER_UNSPECIFIED), byteRestricted_(true) {}

MemoryState::MemoryState(Purpose p, const std::string &name, const SValuePtr &valProtoval)
    : AddressSpace(p, name), addrProtoval_(valProtoval), valProtoval_(valProtoval),
      byteOrder_(ByteOrder::ORDER_UNSPECIFIED), byteRestricted_(true) {
    ASSERT_not_null(valProtoval);
}

MemoryState::MemoryState(const SValue::Ptr &addrProtoval, const SValue::Ptr &valProtoval)
    : AddressSpace(AddressSpace::Purpose::MEMORY, "memory"), addrProtoval_(addrProtoval), valProtoval_(valProtoval),
      byteOrder_(ByteOrder::ORDER_UNSPECIFIED), byteRestricted_(true) {
    ASSERT_not_null(addrProtoval);
    ASSERT_not_null(valProtoval);
}

MemoryState::MemoryState(const MemoryState::Ptr &other)
    : AddressSpace(AddressSpace::Purpose::MEMORY, "memory"), addrProtoval_(other->addrProtoval_), valProtoval_(other->valProtoval_),
      byteOrder_(ByteOrder::ORDER_UNSPECIFIED), merger_(other->merger_), byteRestricted_(other->byteRestricted_) {}

MemoryState::~MemoryState() {}

MemoryState::Ptr
MemoryState::promote(const AddressSpace::Ptr &x) {
    Ptr retval = as<MemoryState>(x);
    ASSERT_not_null(retval);
    return retval;
}

Merger::Ptr
MemoryState::merger() const {
    return merger_;
}

void
MemoryState::merger(const Merger::Ptr &m) {
    merger_ = m;
}

SValue::Ptr
MemoryState::get_addr_protoval() const {
    return addrProtoval_;
}

SValue::Ptr
MemoryState::get_val_protoval() const {
    return valProtoval_;
}

bool
MemoryState::byteRestricted() const {
    return byteRestricted_;
}

void
MemoryState::byteRestricted(bool b) {
    byteRestricted_ = b;
}

ByteOrder::Endianness
MemoryState::get_byteOrder() const {
    return byteOrder_;
}

void
MemoryState::set_byteOrder(ByteOrder::Endianness bo) {
    byteOrder_ = bo;
}

SValuePtr
MemoryState::readLocal(uint8_t index) {
    (void) index;
    ASSERT_require2(false, "No implementation of not base class MemoryState::readLocal\n");
    return {};
}

void
MemoryState::writeLocal(uint8_t index, const SValuePtr &value){
    (void) index;  (void) value;
    ASSERT_require2(false, "No implementation of not base class MemoryState::writeLocal\n");
}

void
MemoryState::pushOperand(const SValuePtr &value) {
    (void) value;
    ASSERT_require2(false, "No implementation of not base class MemoryState::pushOperand\n");
}

SValue::Ptr
MemoryState::popOperand() {
    ASSERT_require2(false, "No implementation of not base class MemoryState::popOperand\n");
    return {};
}

} // namespace
} // namespace
} // namespace
} // namespace

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::MemoryState);
#endif

#endif
