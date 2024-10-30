#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/State.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Formatter.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryState.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RegisterState.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RiscOperators.h>

#include <boost/enable_shared_from_this.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace BaseSemantics {

std::ostream&
operator<<(std::ostream &o, const State &x) {
    x.print(o);
    return o;
}

std::ostream&
operator<<(std::ostream &o, const State::WithFormatter &x) {
    x.print(o);
    return o;
}

State::State() {}

State::State(const RegisterState::Ptr &registers, const MemoryState::Ptr &memory, const RegisterState::Ptr &interrupts)
    : registers_(registers), memory_(memory), interrupts_(interrupts) {
    ASSERT_not_null(registers);
    ASSERT_not_null(memory);
    ASSERT_not_null(interrupts);
    protoval_ = registers->protoval();
    ASSERT_not_null(protoval_);
}

State::State(const RegisterState::Ptr &registers, const MemoryState::Ptr &memory)
    : registers_(registers), memory_(memory) {
    ASSERT_not_null(registers);
    ASSERT_not_null(memory);
    protoval_ = registers->protoval();
    ASSERT_not_null(protoval_);
}

State::State(const State &other)
    : boost::enable_shared_from_this<State>(other), protoval_(other.protoval_) {
    registers_ = other.registers_->clone();
    memory_ = other.memory_->clone();
    if (other.interrupts_)
        interrupts_ = other.interrupts_->clone();
}

State::~State() {}

State::Ptr
State::instance(const RegisterState::Ptr &registers, const MemoryState::Ptr &memory, const RegisterStatePtr &interrupts) {
    return Ptr(new State(registers, memory, interrupts));
}

State::Ptr
State::instance(const RegisterState::Ptr &registers, const MemoryState::Ptr &memory) {
    return Ptr(new State(registers, memory));
}

State::Ptr
State::instance(const StatePtr &other) {
    return Ptr(new State(*other));
}

State::Ptr
State::create(const RegisterStatePtr &registers, const MemoryStatePtr &memory) const {
    return instance(registers, memory);
}

State::Ptr
State::clone() const {
    State::Ptr self = boost::const_pointer_cast<State>(shared_from_this());
    return instance(self);
}

State::Ptr
State::promote(const State::Ptr &x) {
    ASSERT_not_null(x);
    return x;
}

SValue::Ptr
State::protoval() const {
    return protoval_;
}

void
State::clear() {
    registers_->clear();
    memory_->clear();
}

void
State::zeroRegisters() {
    registers_->zero();
}

void
State::clearMemory() {
    memory_->clear();
}

RegisterState::Ptr
State::registerState() const {
    return registers_;
}

MemoryState::Ptr
State::memoryState() const {
    return memory_;
}

bool
State::hasInterruptState() const {
    return interrupts_ != nullptr;
}

RegisterState::Ptr
State::interruptState() const {
    return interrupts_;
}

void
State::interruptState(const RegisterState::Ptr &x) {
    interrupts_ = x;
}

SValue::Ptr
State::readRegister(RegisterDescriptor desc, const SValue::Ptr &dflt, RiscOperators *ops) {
    ASSERT_forbid(desc.isEmpty());
    ASSERT_not_null(dflt);
    ASSERT_not_null(ops);
    return registers_->readRegister(desc, dflt, ops);
}

SValue::Ptr
State::peekRegister(RegisterDescriptor desc, const SValue::Ptr &dflt, RiscOperators *ops) {
    ASSERT_forbid(desc.isEmpty());
    ASSERT_not_null(dflt);
    ASSERT_not_null(ops);
    return registers_->peekRegister(desc, dflt, ops);
}

void
State::writeRegister(RegisterDescriptor desc, const SValue::Ptr &value, RiscOperators *ops) {
    ASSERT_forbid(desc.isEmpty());
    ASSERT_not_null(value);
    ASSERT_not_null(ops);
    registers_->writeRegister(desc, value, ops);
}

SValue::Ptr
State::readMemory(const SValue::Ptr &address, const SValue::Ptr &dflt, RiscOperators *addrOps, RiscOperators *valOps) {
    ASSERT_not_null(address);
    ASSERT_not_null(dflt);
    ASSERT_not_null(addrOps);
    ASSERT_not_null(valOps);
    return memory_->readMemory(address, dflt, addrOps, valOps);
}

SValue::Ptr
State::peekMemory(const SValue::Ptr &address, const SValue::Ptr &dflt, RiscOperators *addrOps, RiscOperators *valOps) {
    ASSERT_not_null(address);
    ASSERT_not_null(dflt);
    ASSERT_not_null(addrOps);
    ASSERT_not_null(valOps);
    return memory_->peekMemory(address, dflt, addrOps, valOps);
}

void
State::writeMemory(const SValue::Ptr &addr, const SValue::Ptr &value, RiscOperators *addrOps, RiscOperators *valOps) {
    ASSERT_not_null(addr);
    ASSERT_not_null(value);
    ASSERT_not_null(addrOps);
    ASSERT_not_null(valOps);
    memory_->writeMemory(addr, value, addrOps, valOps);
}

SValue::Ptr
State::readInterrupt(unsigned major, unsigned minor, const SValue::Ptr &dflt, RiscOperators *ops) {
    ASSERT_not_null(dflt);
    ASSERT_not_null(ops);
    if (interrupts_) {
        return interrupts_->readRegister(RegisterDescriptor(major, minor, 0, 1), dflt, ops);
    } else {
        return {};
    }
}

SValue::Ptr
State::peekInterrupt(unsigned major, unsigned minor, const SValue::Ptr &dflt, RiscOperators *ops) {
    ASSERT_not_null(dflt);
    ASSERT_not_null(ops);
    if (interrupts_) {
        return interrupts_->peekRegister(RegisterDescriptor(major, minor, 0, 1), dflt, ops);
    } else {
        return {};
    }
}

bool
State::writeInterrupt(unsigned major, unsigned minor, const SValue::Ptr &value, RiscOperators *ops) {
    ASSERT_not_null(value);
    ASSERT_not_null(ops);
    if (interrupts_) {
        interrupts_->writeRegister(RegisterDescriptor(major, minor, 0, 1), value, ops);
        return true;
    } else {
        return false;
    }
}

SValue::Ptr
State::raiseInterrupt(unsigned major, unsigned minor, RiscOperators *ops) {
    ASSERT_not_null(ops);
    if (SValue::Ptr retval = readInterrupt(major, minor, ops->undefined_(1), ops)) {
        writeInterrupt(major, minor, ops->boolean_(true), ops);
        return retval;
    } else {
        return {};
    }
}

SValue::Ptr
State::clearInterrupt(unsigned major, unsigned minor, RiscOperators *ops) {
    ASSERT_not_null(ops);
    if (SValue::Ptr retval = readInterrupt(major, minor, ops->undefined_(1), ops)) {
        writeInterrupt(major, minor, ops->boolean_(false), ops);
        return retval;
    } else {
        return {};
    }
}

bool
State::isInterruptDefinitelyRaised(unsigned major, unsigned minor, RiscOperators *ops) {
    ASSERT_not_null(ops);
    SValue::Ptr v = peekInterrupt(major, minor, ops->undefined_(1), ops);
    return v && v->toUnsigned().orElse(0) == 1;
}

bool
State::isInterruptDefinitelyClear(unsigned major, unsigned minor, RiscOperators *ops) {
    ASSERT_not_null(ops);
    SValue::Ptr v = peekInterrupt(major, minor, ops->undefined_(1), ops);
    return v && v->toUnsigned().orElse(1) == 0;
}

void
State::printRegisters(std::ostream &stream, const std::string &prefix) {
    Formatter fmt;
    fmt.set_line_prefix(prefix);
    printRegisters(stream, fmt);
}

void
State::printRegisters(std::ostream &stream, Formatter &fmt) const {
    registers_->print(stream, fmt);
}

void
State::printMemory(std::ostream &stream, const std::string &prefix) const {
    Formatter fmt;
    fmt.set_line_prefix(prefix);
    printMemory(stream, fmt);
}

void
State::printMemory(std::ostream &stream, Formatter &fmt) const {
    memory_->print(stream, fmt);
}

void
State::printInterrupts(std::ostream &stream, const std::string &prefix) {
    Formatter fmt;
    fmt.set_line_prefix(prefix);
    printInterrupts(stream, fmt);
}

void
State::printInterrupts(std::ostream &stream, Formatter &fmt) const {
    if (interrupts_)
        interrupts_->print(stream, fmt);
}

bool
State::merge(const State::Ptr &other, RiscOperators *ops) {
    bool memoryChanged = memoryState()->merge(other->memoryState(), ops, ops);
    bool registersChanged = registerState()->merge(other->registerState(), ops);
    bool interruptsChanged = interruptState() && other->interruptState() && interruptState()->merge(other->interruptState(), ops);
    return memoryChanged || registersChanged || interruptsChanged;
}

void
State::hash(Combinatorics::Hasher &hasher, RiscOperators *addrOps, RiscOperators *valOps) const {
    registerState()->hash(hasher, valOps);
    memoryState()->hash(hasher, addrOps, valOps);
    interruptState()->hash(hasher, valOps);
}

void
State::print(std::ostream &stream, const std::string &prefix) const {
    Formatter fmt;
    fmt.set_line_prefix(prefix);
    print(stream, fmt);
}

void
State::print(std::ostream &stream, Formatter &fmt) const
{
    std::string prefix = fmt.get_line_prefix();
    Indent indent(fmt);
    stream <<prefix <<"registers:\n" <<(*registers_+fmt)
           <<prefix <<"memory:\n" <<(*memory_+fmt);
    if (interrupts_)
        stream <<prefix <<"interrupts:\n" <<(*interrupts_+fmt);
}

std::string
State::toString() const {
    std::ostringstream ss;
    print(ss, "  ");
    return ss.str();
}

State::WithFormatter
State::operator+(const std::string &linePrefix) {
    static Formatter fmt;
    fmt.set_line_prefix(linePrefix);
    return with_format(fmt);
}

} // namespace
} // namespace
} // namespace
} // namespace

BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::State);

#endif
