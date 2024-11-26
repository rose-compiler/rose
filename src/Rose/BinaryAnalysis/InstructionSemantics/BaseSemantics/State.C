#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/State.h>

#include <Rose/Affirm.h>
#include <Rose/As.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/AddressSpace.h>
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

State::State(const RegisterState::Ptr &registers, const MemoryState::Ptr &memory, const RegisterState::Ptr &interrupts) {
    protoval_ = notnull(notnull(registers)->protoval());

    ASSERT_not_null(registers);
    ASSERT_require(registers->purpose() == AddressSpace::Purpose::REGISTERS);
    insertAddressSpace(registers);

    ASSERT_not_null(memory);
    ASSERT_require(memory->purpose() == AddressSpace::Purpose::MEMORY);
    insertAddressSpace(memory);

    ASSERT_not_null(interrupts);
    ASSERT_require(interrupts->purpose() == AddressSpace::Purpose::INTERRUPTS);
    insertAddressSpace(interrupts);
}

State::State(const RegisterState::Ptr &registers, const MemoryState::Ptr &memory) {
    protoval_ = notnull(notnull(registers)->protoval());

    ASSERT_not_null(registers);
    ASSERT_require(registers->purpose() == AddressSpace::Purpose::REGISTERS);
    insertAddressSpace(registers);

    ASSERT_not_null(memory);
    ASSERT_require(memory->purpose() == AddressSpace::Purpose::MEMORY);
    insertAddressSpace(memory);
}

State::State(const State &other)
    : boost::enable_shared_from_this<State>(other), protoval_(other.protoval_) {
    for (const AddressSpace::Ptr &space: other.addressSpaces())
        insertAddressSpace(space->clone());
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
State::insertAddressSpace(const AddressSpace::Ptr &space) {
    ASSERT_not_null(space);
    ASSERT_require2(std::find(addressSpaces_.begin(), addressSpaces_.end(), space) == addressSpaces_.end(),
                    "an address space can only appear once per semantic state");

    // Some address spaces need to be a particular type (at least for now)
    ASSERT_require(space->purpose() != AddressSpace::Purpose::REGISTERS || as<RegisterState>(space));
    ASSERT_require(space->purpose() != AddressSpace::Purpose::MEMORY || as<MemoryState>(space));
    ASSERT_require(space->purpose() != AddressSpace::Purpose::INTERRUPTS || as<RegisterState>(space));

    addressSpaces_.push_back(space);
}

const std::vector<AddressSpace::Ptr>&
State::addressSpaces() const {
    return addressSpaces_;
}

AddressSpace::Ptr
State::findFirstAddressSpace(const AddressSpace::Purpose purpose) const {
    for (const AddressSpace::Ptr &space: addressSpaces()) {
        if (space->purpose() == purpose)
            return space;
    }
    return {};
}

AddressSpace::Ptr
State::findFirstAddressSpace(const AddressSpace::Purpose purpose, const std::string &name) const {
    for (const AddressSpace::Ptr &space: addressSpaces()) {
        if (space->purpose() == purpose && space->name() == name)
            return space;
    }
    return {};
}

SValue::Ptr
State::read(const AddressSpace::Ptr &space, const AddressSpaceAddress &addr, const SValue::Ptr &dflt,
            RiscOperators &addrOps, RiscOperators &valOps) {
    ASSERT_not_null(space);
    ASSERT_forbid(addr.isEmpty());
    ASSERT_not_null(dflt);
    ASSERT_require(dflt->nBits() > 0);
    return space->read(addr, dflt, addrOps, valOps);
}

SValue::Ptr
State::peek(const AddressSpace::Ptr &space, const AddressSpaceAddress &addr, const SValue::Ptr &dflt,
            RiscOperators &addrOps, RiscOperators &valOps) {
    ASSERT_not_null(space);
    ASSERT_forbid(addr.isEmpty());
    ASSERT_not_null(dflt);
    ASSERT_require(dflt->nBits() > 0);
    return space->peek(addr, dflt, addrOps, valOps);
}

void
State::write(const AddressSpace::Ptr &space, const AddressSpaceAddress &addr, const SValue::Ptr &dflt,
             RiscOperators &addrOps, RiscOperators &valOps) {
    ASSERT_not_null(space);
    ASSERT_forbid(addr.isEmpty());
    ASSERT_not_null(dflt);
    ASSERT_require(dflt->nBits() > 0);
    space->write(addr, dflt, addrOps, valOps);
}

bool
State::merge(const State::Ptr &other, RiscOperators *addrOps, RiscOperators *valOps) {
    ASSERT_not_null(other);
    ASSERT_not_null(addrOps);
    ASSERT_not_null(valOps);

    bool changed = false;
    for (const AddressSpace::Ptr &otherSpace: other->addressSpaces()) {
        if (const AddressSpace::Ptr &thisSpace = findFirstAddressSpace(otherSpace->purpose(), otherSpace->name())) {
            if (thisSpace->merge(otherSpace, addrOps, valOps))
                changed = true;
        } else {
            insertAddressSpace(otherSpace->clone());
            changed = true;
        }
    }
    return changed;
}

void
State::clear() {
    for (const AddressSpace::Ptr &space: addressSpaces())
        space->clear();
}

void
State::zeroRegisters() {
    for (const AddressSpace::Ptr &space: addressSpaces()) {
        if (space->purpose() == AddressSpace::Purpose::REGISTERS) {
            if (auto regs = as<RegisterState>(space))
                regs->zero();
        }
    }
}

void
State::clearMemory() {
    for (const AddressSpace::Ptr &space: addressSpaces()) {
        if (space->purpose() == AddressSpace::Purpose::MEMORY) {
            if (auto mem = as<MemoryState>(space))
                space->clear();
        }
    }
}

RegisterState::Ptr
State::registerState() const {
    for (const AddressSpace::Ptr &space: addressSpaces()) {
        if (space->purpose() == AddressSpace::Purpose::REGISTERS) {
            if (auto retval = as<RegisterState>(space))
                return retval;
        }
    }
    return {};
}

MemoryState::Ptr
State::memoryState() const {
    for (const AddressSpace::Ptr &space: addressSpaces()) {
        if (space->purpose() == AddressSpace::Purpose::MEMORY) {
            if (auto retval = as<MemoryState>(space))
                return retval;
        }
    }
    return {};
}

RegisterState::Ptr
State::interruptState() const {
    for (const AddressSpace::Ptr &space: addressSpaces()) {
        if (space->purpose() == AddressSpace::Purpose::INTERRUPTS) {
            if (auto retval = as<RegisterState>(space))
                return retval;
        }
    }
    return {};
}

void
State::interruptState(const RegisterState::Ptr &x) {
    for (auto space = addressSpaces_.begin(); space != addressSpaces_.end(); ++space) {
        if ((*space)->purpose() == AddressSpace::Purpose::INTERRUPTS) {
            if (x) {
                *space = x;
            } else {
                addressSpaces_.erase(space);
            }
            return;
        }
    }
}

bool
State::hasInterruptState() const {
    return interruptState() != nullptr;
}

SValue::Ptr
State::readRegister(RegisterDescriptor desc, const SValue::Ptr &dflt, RiscOperators *ops) {
    ASSERT_forbid(desc.isEmpty());
    ASSERT_not_null(dflt);
    ASSERT_not_null(ops);
    return notnull(registerState())->readRegister(desc, dflt, ops);
}

SValue::Ptr
State::peekRegister(RegisterDescriptor desc, const SValue::Ptr &dflt, RiscOperators *ops) {
    ASSERT_forbid(desc.isEmpty());
    ASSERT_not_null(dflt);
    ASSERT_not_null(ops);
    return notnull(registerState())->peekRegister(desc, dflt, ops);
}

void
State::writeRegister(RegisterDescriptor desc, const SValue::Ptr &value, RiscOperators *ops) {
    ASSERT_forbid(desc.isEmpty());
    ASSERT_not_null(value);
    ASSERT_not_null(ops);
    notnull(registerState())->writeRegister(desc, value, ops);
}

SValue::Ptr
State::readMemory(const SValue::Ptr &address, const SValue::Ptr &dflt, RiscOperators *addrOps, RiscOperators *valOps) {
    ASSERT_not_null(address);
    ASSERT_not_null(dflt);
    ASSERT_not_null(addrOps);
    ASSERT_not_null(valOps);
    return notnull(memoryState())->readMemory(address, dflt, addrOps, valOps);
}

SValue::Ptr
State::peekMemory(const SValue::Ptr &address, const SValue::Ptr &dflt, RiscOperators *addrOps, RiscOperators *valOps) {
    ASSERT_not_null(address);
    ASSERT_not_null(dflt);
    ASSERT_not_null(addrOps);
    ASSERT_not_null(valOps);
    return notnull(memoryState())->peekMemory(address, dflt, addrOps, valOps);
}

void
State::writeMemory(const SValue::Ptr &addr, const SValue::Ptr &value, RiscOperators *addrOps, RiscOperators *valOps) {
    ASSERT_not_null(addr);
    ASSERT_not_null(value);
    ASSERT_not_null(addrOps);
    ASSERT_not_null(valOps);
    notnull(memoryState())->writeMemory(addr, value, addrOps, valOps);
}

SValue::Ptr
State::readInterrupt(unsigned major, unsigned minor, const SValue::Ptr &dflt, RiscOperators *ops) {
    ASSERT_not_null(dflt);
    ASSERT_not_null(ops);
    if (auto interrupts = interruptState()) {
        return interrupts->readRegister(RegisterDescriptor(major, minor, 0, 1), dflt, ops);
    } else {
        return {};
    }
}

SValue::Ptr
State::peekInterrupt(unsigned major, unsigned minor, const SValue::Ptr &dflt, RiscOperators *ops) {
    ASSERT_not_null(dflt);
    ASSERT_not_null(ops);
    if (auto interrupts = interruptState()) {
        return interrupts->peekRegister(RegisterDescriptor(major, minor, 0, 1), dflt, ops);
    } else {
        return {};
    }
}

bool
State::writeInterrupt(unsigned major, unsigned minor, const SValue::Ptr &value, RiscOperators *ops) {
    ASSERT_not_null(value);
    ASSERT_not_null(ops);
    if (auto interrupts = interruptState()) {
        interrupts->writeRegister(RegisterDescriptor(major, minor, 0, 1), value, ops);
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
    notnull(registerState())->print(stream, fmt);
}

void
State::printMemory(std::ostream &stream, const std::string &prefix) const {
    Formatter fmt;
    fmt.set_line_prefix(prefix);
    printMemory(stream, fmt);
}

void
State::printMemory(std::ostream &stream, Formatter &fmt) const {
    notnull(memoryState())->print(stream, fmt);
}

void
State::printInterrupts(std::ostream &stream, const std::string &prefix) {
    Formatter fmt;
    fmt.set_line_prefix(prefix);
    printInterrupts(stream, fmt);
}

void
State::printInterrupts(std::ostream &stream, Formatter &fmt) const {
    if (auto interrupts = interruptState())
        interrupts->print(stream, fmt);
}

void
State::hash(Combinatorics::Hasher &hasher, RiscOperators *addrOps, RiscOperators *valOps) const {
    for (const AddressSpace::Ptr &space: addressSpaces())
        space->hash(hasher, addrOps, valOps);
}

void
State::print(std::ostream &stream, const std::string &prefix) const {
    Formatter fmt;
    fmt.set_line_prefix(prefix);
    print(stream, fmt);
}

void
State::print(std::ostream &stream, Formatter &fmt) const {
    std::string prefix = fmt.get_line_prefix();
    Indent indent(fmt);
    for (const AddressSpace::Ptr &space: addressSpaces()) {
        stream <<prefix <<space->printableName() <<":\n";
        stream <<prefix <<(*space + fmt);
    }
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
