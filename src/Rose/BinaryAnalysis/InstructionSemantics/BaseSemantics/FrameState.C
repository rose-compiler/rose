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

//FIX_ME by using value from the class (may not be easy to get)
#define MAX_LOCALS 256

FrameState::~FrameState() {}

FrameState::FrameState() : MemoryState() {
    purpose(AddressSpace::Purpose::FRAMES);
    name("frame");
}

FrameState::FrameState(const SValue::Ptr &valProtoval)
  : MemoryState(valProtoval, valProtoval), locals_(MAX_LOCALS) {
    purpose(AddressSpace::Purpose::FRAMES);
    name("frame");
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

SValue::Ptr
FrameState::readLocal(uint8_t index) {
     ASSERT_require(index < locals_.size());
     return locals_[index]; // may be null if not yet initialized
}

void
FrameState::writeLocal(uint8_t index, const SValuePtr &value) {
    ASSERT_require(index < locals_.size());
    locals_[index] = value;
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

    if (stack_.size()) {
        out << "  operands:\n";
    }

    //TODO::use formatter
    for (auto val : stack_) {
        if (val != nullptr) {
            out << "    " << *val << "\n";
        } else {
            out << "    NULL\n";
        }
    }

    bool first{true};
    for (size_t i = 0; i < locals_.size(); ++i) {
        auto val = locals_[i];
        if (val != nullptr) {
            if (first) {
                out << "  locals:\n";
                first = false;
            }
            out << "    " << i << ": " << *val << "\n";
        }
    }
}


} // namespace
} // namespace
} // namespace
} // namespace

#endif
