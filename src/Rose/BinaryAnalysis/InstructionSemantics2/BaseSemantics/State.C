#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/State.h>

#include <Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/Formatter.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/MemoryState.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/RegisterState.h>

#include <boost/enable_shared_from_this.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
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

State::State(const RegisterStatePtr &registers, const MemoryStatePtr &memory)
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
}

State::~State() {}

SValuePtr
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

SValuePtr
State::readRegister(RegisterDescriptor desc, const SValuePtr &dflt, RiscOperators *ops) {
    ASSERT_forbid(desc.isEmpty());
    ASSERT_not_null(dflt);
    ASSERT_not_null(ops);
    return registers_->readRegister(desc, dflt, ops);
}

SValuePtr
State::peekRegister(RegisterDescriptor desc, const SValuePtr &dflt, RiscOperators *ops) {
    ASSERT_forbid(desc.isEmpty());
    ASSERT_not_null(dflt);
    ASSERT_not_null(ops);
    return registers_->peekRegister(desc, dflt, ops);
}

void
State::writeRegister(RegisterDescriptor desc, const SValuePtr &value, RiscOperators *ops) {
    ASSERT_forbid(desc.isEmpty());
    ASSERT_not_null(value);
    ASSERT_not_null(ops);
    registers_->writeRegister(desc, value, ops);
}

SValuePtr
State::readMemory(const SValuePtr &address, const SValuePtr &dflt, RiscOperators *addrOps, RiscOperators *valOps) {
    ASSERT_not_null(address);
    ASSERT_not_null(dflt);
    ASSERT_not_null(addrOps);
    ASSERT_not_null(valOps);
    return memory_->readMemory(address, dflt, addrOps, valOps);
}

SValuePtr
State::peekMemory(const SValuePtr &address, const SValuePtr &dflt, RiscOperators *addrOps, RiscOperators *valOps) {
    ASSERT_not_null(address);
    ASSERT_not_null(dflt);
    ASSERT_not_null(addrOps);
    ASSERT_not_null(valOps);
    return memory_->peekMemory(address, dflt, addrOps, valOps);
}

void
State::writeMemory(const SValuePtr &addr, const SValuePtr &value, RiscOperators *addrOps, RiscOperators *valOps) {
    ASSERT_not_null(addr);
    ASSERT_not_null(value);
    ASSERT_not_null(addrOps);
    ASSERT_not_null(valOps);
    memory_->writeMemory(addr, value, addrOps, valOps);
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

bool
State::merge(const StatePtr &other, RiscOperators *ops) {
    bool memoryChanged = memoryState()->merge(other->memoryState(), ops, ops);
    bool registersChanged = registerState()->merge(other->registerState(), ops);
    return memoryChanged || registersChanged;
}

void
State::hash(Combinatorics::Hasher &hasher, RiscOperators *addrOps, RiscOperators *valOps) const {
    registerState()->hash(hasher, valOps);
    memoryState()->hash(hasher, addrOps, valOps);
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
    stream <<prefix <<"registers:\n" <<(*registers_+fmt) <<prefix <<"memory:\n" <<(*memory_+fmt);
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

BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::State);

#endif
