#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/FrameState.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SymbolicMemory.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SValue.h>
#include <Rose/StringUtility/NumberToString.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace BaseSemantics {

FrameState::~FrameState() {}

FrameState::FrameState(const SValue::Ptr &valProtoval)
    : MemoryState(valProtoval, valProtoval) {
}

FrameState::Ptr
FrameState::instance(const SValue::Ptr &valProtoval) {
    return Ptr(new FrameState(valProtoval));
}

MemoryState::Ptr
FrameState::create(const SValuePtr &addrProtoval, const SValuePtr &valProtoval) const {
    (void) addrProtoval;
    return instance(valProtoval);
}

AddressSpace::Ptr
FrameState::clone() const {
    ASSERT_require2(false, "TODO:FrameState::clone\n");
    return {};
}

FrameState::Ptr
FrameState::promote(const AddressSpace::Ptr &x) {
    (void) x;
    ASSERT_require2(false, "TODO:FrameState::promote\n");
    return {};
}

void
FrameState::clear() {
    stack_.clear();
}

SValue::Ptr
FrameState::readMemory(const SValue::Ptr &address_, const SValue::Ptr &dflt,
                       RiscOperators */*addrOps*/, RiscOperators */*valOps*/) {
    (void) address_;
    (void) dflt;
    ASSERT_require2(false, "TODO::readMemory\n");
    return {};
}

SValue::Ptr
FrameState::peekMemory(const SValue::Ptr &address, const SValue::Ptr &dflt, RiscOperators *addrOps, RiscOperators *valOps) {
    ASSERT_require2(false, "TODO::peekMemory\n");
    return readMemory(address, dflt, addrOps, valOps);  // readMemory doesn't have side effects
}

void
FrameState::writeMemory(const SValue::Ptr &address_, const SValue::Ptr &value_, RiscOperators */*addrOps*/, RiscOperators */*valOps*/) {
    (void) address_;
    (void) value_;
    ASSERT_require2(false, "TODO::writeMemory\n");
}

void
FrameState::pushOperand(const SValuePtr &value) {
    stack_.push_back(value);
}

SValue::Ptr
FrameState::popOperand() {
    auto val = stack_.back();  // get value
    stack_.pop_back();         // remove it
    return val;
}

bool
FrameState::merge(const AddressSpace::Ptr &other_, RiscOperators */*addrOps*/, RiscOperators */*valOps*/) {
    (void) other_;
    ASSERT_require2(false, "TODO::merge\n");
}

void
FrameState::hash(Combinatorics::Hasher &hasher, RiscOperators*, RiscOperators*) const {
    (void) hasher;
    ASSERT_require2(false, "TODO::hash\n");
}

void
FrameState::print(std::ostream &out, Formatter &formatter_) const {
    (void) out;
    (void) formatter_;

    //TODO::use formatter
    for (auto val : stack_) {
        out << "  " << *val << "\n";
    }
}


} // namespace
} // namespace
} // namespace
} // namespace

#endif
