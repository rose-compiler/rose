#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/FrameState.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SymbolicMemory.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SValue.h>
#include <Rose/StringUtility/NumberToString.h>

#include <iostream>

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
    : MemoryState(valProtoval, valProtoval), locals_{} {
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
FrameState::readLocal(uint8_t index) const {
    auto found = locals_.find(index);
    if (found != locals_.end()) {
        return found->second;
    }
    return {};
}

void
FrameState::writeLocal(uint8_t index, const SValuePtr &sval) {
    locals_[index] = sval;
}

SValue::Ptr
FrameState::peekOperand() {
    ASSERT_require2(stack_.size() >= 1, "ERROR: operand stack is empty\n");
    return stack_.back();
}

SValue::Ptr
FrameState::popOperand() {
    ASSERT_require2(stack_.size() >= 1, "ERROR: operand stack is empty\n");
    auto val = stack_.back();  // get value
    stack_.pop_back();          // remove it
    return val;
}

void
FrameState::pushOperand(const SValuePtr &sval) {
    stack_.push_back(sval);
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
    for (auto sval : stack_) {
        if (sval != nullptr) {
            out << "    ";
            if (sval->kind() == JvmValueKind::Integer) {
                out << *sval << ":Integer";
            }
            else if (sval->kind() == JvmValueKind::Long) {
                out << *sval << ":Long";
            }
            else if (sval->kind() == JvmValueKind::Float) {
                out << *sval << ":Float";
            }
            else if (sval->kind() == JvmValueKind::Double) {
                out << *sval << ":Double";
            }
            else if (sval->kind() == JvmValueKind::ArrayReference) {
                SValuePtr count = sval->arrayLength();
                out << "ArrayReference[" << *count << "]";
                if (sval->hasTypeDescriptor()) {
                    out << ":" << sval->typeDescriptor();
                }
                out << "\n";
            }
            out << "\n";
        } else {
            out << "    NULL\n";
        }
    }

    bool first{true};

//TODO:Print and test category-2 types
#if 0
local[1] = Long value
local[2] = unusable/continuation slot
#endif

    for (const auto &[idx, sval] : locals_) {
        if (first) {
            out << "  locals:\n";
            first = false;
        }
        if (sval != nullptr) {
            out << "    " << idx << ": ";
            if (sval->kind() == JvmValueKind::Integer) {
                out << *sval << ":Integer";
            }
            else if (sval->kind() == JvmValueKind::Long) {
                out << *sval << ":Long";
            }
            else if (sval->kind() == JvmValueKind::Float) {
                out << *sval << ":Float";
            }
            else if (sval->kind() == JvmValueKind::Double) {
                out << *sval << ":Double";
            }
            else if (sval->kind() == JvmValueKind::ArrayReference) {
                SValuePtr count = sval->arrayLength();
                out << "ArrayReference[" << *count << "]";
                if (sval->hasTypeDescriptor()) {
                    out << ":" << sval->typeDescriptor();
                }
                out << "\n";
            }
            out << "\n";
        }
    }
}


} // namespace
} // namespace
} // namespace
} // namespace

#endif
