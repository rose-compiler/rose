#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/NativeSemantics.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>

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

BaseSemantics::SValuePtr
RegisterState::peekRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &dflt, BaseSemantics::RiscOperators *ops) {
    ASSERT_not_null(process_);
    Sawyer::Container::BitVector value = process_->readRegister(reg);
    return protoval()->number_(value.size(), value.toInteger());
}

void
RegisterState::writeRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &value, BaseSemantics::RiscOperators *ops) {
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

BaseSemantics::SValuePtr
MemoryState::peekMemory(const BaseSemantics::SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                        BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps) {
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dispatcher
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Dispatcher::processInstruction(SgAsmInstruction *insn) {
    ASSERT_not_null(insn);
    processInstruction(insn->get_address());
}

void
Dispatcher::processInstruction(rose_addr_t va) {
    ASSERT_not_null(process_);
    process_->executionAddress(va);
    process_->singleStep();
}

SgAsmInstruction*
Dispatcher::currentInstruction() const {
    ASSERT_not_null(process_);
    rose_addr_t va = process_->executionAddress();
    uint8_t buf[16];
    size_t nRead = process_->readMemory(va, sizeof buf, buf);
    if (0 == nRead)
        return NULL;
    return process_->disassembler()->disassembleOne(buf, va, nRead, va);
}

RegisterDescriptor
Dispatcher::instructionPointerRegister() const {
    ASSERT_not_null(process_);
    return process_->disassembler()->instructionPointerRegister();
}

RegisterDescriptor
Dispatcher::stackPointerRegister() const {
    ASSERT_not_null(process_);
    return process_->disassembler()->stackPointerRegister();
}

RegisterDescriptor
Dispatcher::stackFrameRegister() const {
    ASSERT_not_null(process_);
    return process_->disassembler()->stackFrameRegister();
}

RegisterDescriptor
Dispatcher::callReturnRegister() const {
    ASSERT_not_null(process_);
    return process_->disassembler()->callReturnRegister();
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
