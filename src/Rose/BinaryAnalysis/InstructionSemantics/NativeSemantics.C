#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS) && defined(ROSE_ENABLE_DEBUGGER_LINUX)
#include <Rose/BinaryAnalysis/InstructionSemantics/NativeSemantics.h>

#include <Rose/As.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

#include <SgAsmInstruction.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace NativeSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SValue
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Register state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RegisterState::RegisterState() {}

RegisterState::RegisterState(const BaseSemantics::SValue::Ptr &protoval, const Debugger::Linux::Ptr &process)
    : BaseSemantics::RegisterState(protoval, process->registerDictionary()), process_(process) {
    ASSERT_not_null(process);
}

RegisterState::~RegisterState() {}

RegisterState::Ptr
RegisterState::instance() {
    return Ptr(new RegisterState);
}

RegisterState::Ptr
RegisterState::instance(const BaseSemantics::SValue::Ptr &protoval, const Debugger::Linux::Ptr &process) {
    ASSERT_not_null(protoval);
    (void) SValue::promote(protoval);
    return Ptr(new RegisterState(protoval, process));
}

BaseSemantics::RegisterState::Ptr
RegisterState::create(const BaseSemantics::SValue::Ptr &/*protoval*/, const RegisterDictionary::Ptr&) const {
    ASSERT_not_implemented("not applicable for this class");
}

BaseSemantics::RegisterState::Ptr
RegisterState::clone() const {
    ASSERT_not_implemented("not applicable for this class");
}

RegisterState::Ptr
RegisterState::promote(const BaseSemantics::RegisterState::Ptr &x) {
    Ptr retval = as<RegisterState>(x);
    ASSERT_not_null(retval);
    return retval;
}

Debugger::Linux::Ptr
RegisterState::process() const {
    return process_;
}

BaseSemantics::SValue::Ptr
RegisterState::peekRegister(RegisterDescriptor reg, const BaseSemantics::SValue::Ptr &/*dflt*/, BaseSemantics::RiscOperators*) {
    ASSERT_not_null(process_);
    Sawyer::Container::BitVector value = process_->readRegister(Debugger::ThreadId::unspecified(), reg);
    return protoval()->number_(value.size(), value.toInteger());
}

void
RegisterState::writeRegister(RegisterDescriptor, const BaseSemantics::SValue::Ptr&, BaseSemantics::RiscOperators*) {
    ASSERT_not_null(process_);
    ASSERT_not_implemented("[Robb Matzke 2019-09-05]: Debugger has no writeRegister method");
}

void
RegisterState::hash(Combinatorics::Hasher&, BaseSemantics::RiscOperators*) const {
    ASSERT_not_implemented("[Robb Matzke 2021-03-26]: would require reading all registers");
}

void
RegisterState::print(std::ostream&, Formatter&) const {
    ASSERT_not_implemented("[Robb Matzke 2019-09-05]: would require reading all registers");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Memory state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MemoryState::MemoryState() {}

MemoryState::~MemoryState() {}

MemoryState::MemoryState(const BaseSemantics::SValue::Ptr &addrProtoval, const BaseSemantics::SValue::Ptr &valProtoval,
                         const Debugger::Linux::Ptr &process)
    : BaseSemantics::MemoryState(addrProtoval, valProtoval), process_(process) {
    ASSERT_not_null(process);
}

MemoryState::Ptr
MemoryState::instance() {
    return Ptr(new MemoryState);
}

MemoryState::Ptr
MemoryState::instance(const BaseSemantics::SValue::Ptr &addrProtoval, const BaseSemantics::SValue::Ptr &valProtoval,
                      const Debugger::Linux::Ptr &process) {
    return Ptr(new MemoryState(addrProtoval, valProtoval, process));
}

BaseSemantics::MemoryState::Ptr
MemoryState::create(const BaseSemantics::SValue::Ptr &/*addrProtoval*/, const BaseSemantics::SValue::Ptr &/*valProtoval*/) const {
    ASSERT_not_implemented("not applicable for this class");
}

BaseSemantics::MemoryState::Ptr
MemoryState::clone() const {
    ASSERT_not_implemented("not applicable for this class");
}

MemoryState::Ptr
MemoryState::promote(const BaseSemantics::MemoryState::Ptr &x) {
    Ptr retval = as<MemoryState>(x);
    ASSERT_not_null(retval);
    return retval;
}

Debugger::Linux::Ptr
MemoryState::process() const {
    return process_;
}

BaseSemantics::SValue::Ptr
MemoryState::peekMemory(const BaseSemantics::SValue::Ptr &address, const BaseSemantics::SValue::Ptr &dflt,
                        BaseSemantics::RiscOperators */*addrOps*/, BaseSemantics::RiscOperators */*valOps*/) {
    ASSERT_not_null(process_);
    std::vector<uint8_t> buffer((dflt->nBits() + 7) / 8);
    ASSERT_require(buffer.size() <= 8);
    rose_addr_t va = SValue::promote(address)->toUnsigned().get();
    size_t nRead = process_->readMemory(va, buffer.size(), &buffer[0]);
    if (nRead != buffer.size())
        throw Exception("short read from process at " + StringUtility::addrToString(va));

    uint64_t value = 0;
    switch (get_byteOrder()) {
        case ByteOrder::ORDER_LSB:
            for (size_t i = 0; i < buffer.size(); ++i)
                value |= (uint64_t)buffer[i] << (8*i);
            break;
        case ByteOrder::ORDER_MSB:
            for (size_t i = 0; i < buffer.size(); ++i)
                value = (value << 8) | (uint64_t)buffer[i];
            break;
        default:
            ASSERT_not_reachable("invalid byte order");
    }

    return get_val_protoval()->number_(8*buffer.size(), value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// State
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RISC operators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RiscOperators::RiscOperators(const BaseSemantics::State::Ptr &state)
    : ConcreteSemantics::RiscOperators(state, SmtSolverPtr()) {
    name("Native");
}

RiscOperators::~RiscOperators() {}

RiscOperators::Ptr
RiscOperators::instanceFromProtoval(const BaseSemantics::SValue::Ptr &protoval, const Debugger::Linux::Ptr &process) {
    RegisterState::Ptr registers = RegisterState::instance(protoval, process);
    MemoryState::Ptr memory = MemoryState::instance(protoval, protoval, process);
    State::Ptr state = State::instance(registers, memory);
    return Ptr(new RiscOperators(state));
}

RiscOperators::Ptr
RiscOperators::instanceFromState(const BaseSemantics::State::Ptr &state) {
    (void) State::promote(state);                   // check that it's the correct type
    return Ptr(new RiscOperators(state));
}

BaseSemantics::RiscOperators::Ptr
RiscOperators::create(const BaseSemantics::SValue::Ptr &/*protoval*/, const SmtSolver::Ptr&) const {
    TODO("[Robb Matzke 2019-09-05]");
}

RiscOperators::Ptr
RiscOperators::promote(const BaseSemantics::RiscOperators::Ptr &x) {
    Ptr retval = as<RiscOperators>(x);
    ASSERT_not_null(retval);
    return retval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dispatcher
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Dispatcher::Dispatcher(const Architecture::Base::ConstPtr &arch, const Debugger::Linux::Ptr &process,
                       const BaseSemantics::SValue::Ptr &protoval)
    : BaseSemantics::Dispatcher(arch), process_(process) {
    ASSERT_require(process_->kernelWordSize() == arch->bitsPerWord());
    operators(RiscOperators::instanceFromProtoval(protoval, process_));
}

Dispatcher::Dispatcher(const Architecture::Base::ConstPtr &arch, const BaseSemantics::RiscOperators::Ptr &ops)
    : BaseSemantics::Dispatcher(arch, ops), process_(RiscOperators::promote(ops)->process()) {
    ASSERT_require(process_->kernelWordSize() == arch->bitsPerWord());
}

Dispatcher::~Dispatcher() {}

Dispatcher::Ptr
Dispatcher::instance(const Architecture::Base::ConstPtr &arch, const Debugger::Linux::Ptr &process,
                     const BaseSemantics::SValue::Ptr &protoval) {
    return Ptr(new Dispatcher(arch, process, protoval));
}

Dispatcher::Ptr
Dispatcher::instance(const Architecture::Base::ConstPtr &arch, const Debugger::Linux::Specimen &specimen,
                     const BaseSemantics::SValue::Ptr &protoval) {
    Debugger::Linux::Ptr process = Debugger::Linux::instance(specimen);
    return Ptr(new Dispatcher(arch, process, protoval));
}

Dispatcher::Ptr
Dispatcher::instance(const Architecture::Base::ConstPtr &arch, const BaseSemantics::RiscOperators::Ptr &ops) {
    (void) RiscOperators::promote(ops);             // check type
    return Ptr(new Dispatcher(arch, ops));
}

BaseSemantics::Dispatcher::Ptr
Dispatcher::create(const BaseSemantics::RiscOperators::Ptr&) const {
    notApplicable("create");
}

void
Dispatcher::processInstruction(SgAsmInstruction *insn) {
    ASSERT_not_null(insn);
    processInstruction(insn->get_address());
}

void
Dispatcher::processInstruction(rose_addr_t va) {
    ASSERT_not_null(process_);
    process_->executionAddress(Debugger::ThreadId::unspecified(), va);
    process_->singleStep(Debugger::ThreadId::unspecified());
}

SgAsmInstruction*
Dispatcher::currentInstruction() const {
    ASSERT_not_null(process_);
    rose_addr_t va = process_->executionAddress(Debugger::ThreadId::unspecified());
    uint8_t buf[16];
    size_t nRead = process_->readMemory(va, sizeof buf, buf);
    if (0 == nRead)
        return NULL;
    return process_->disassembler()->disassembleOne(buf, va, nRead, va);
}

RegisterDescriptor
Dispatcher::instructionPointerRegister() const {
    ASSERT_not_null(process_);
    return process_->disassembler()->architecture()->registerDictionary()->instructionPointerRegister();
}

RegisterDescriptor
Dispatcher::stackPointerRegister() const {
    ASSERT_not_null(process_);
    return process_->disassembler()->architecture()->registerDictionary()->stackPointerRegister();
}

RegisterDescriptor
Dispatcher::stackFrameRegister() const {
    ASSERT_not_null(process_);
    return process_->disassembler()->architecture()->registerDictionary()->stackFrameRegister();
}

RegisterDescriptor
Dispatcher::callReturnRegister() const {
    ASSERT_not_null(process_);
    return process_->disassembler()->architecture()->registerDictionary()->callReturnRegister();
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
