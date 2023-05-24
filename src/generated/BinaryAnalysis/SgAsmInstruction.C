//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmInstruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

std::string const&
SgAsmInstruction::get_mnemonic() const {
    return p_mnemonic;
}

void
SgAsmInstruction::set_mnemonic(std::string const& x) {
    this->p_mnemonic = x;
    set_isModified(true);
}

SgUnsignedCharList const&
SgAsmInstruction::get_raw_bytes() const {
    return p_raw_bytes;
}

void
SgAsmInstruction::set_raw_bytes(SgUnsignedCharList const& x) {
    this->p_raw_bytes = x;
    set_isModified(true);
}

SgAsmOperandList* const&
SgAsmInstruction::get_operandList() const {
    return p_operandList;
}

void
SgAsmInstruction::set_operandList(SgAsmOperandList* const& x) {
    changeChildPointer(this->p_operandList, const_cast<SgAsmOperandList*&>(x));
    set_isModified(true);
}

int64_t const&
SgAsmInstruction::get_stackDeltaIn() const {
    return p_stackDeltaIn;
}

void
SgAsmInstruction::set_stackDeltaIn(int64_t const& x) {
    this->p_stackDeltaIn = x;
    set_isModified(true);
}

SgAsmExprListExp* const&
SgAsmInstruction::get_semantics() const {
    return p_semantics;
}

void
SgAsmInstruction::set_semantics(SgAsmExprListExp* const& x) {
    changeChildPointer(this->p_semantics, const_cast<SgAsmExprListExp*&>(x));
    set_isModified(true);
}

SgAsmInstruction::~SgAsmInstruction() {
    destructorHelper();
}

SgAsmInstruction::SgAsmInstruction()
    : p_operandList(nullptr)
    , p_cacheLockCount(0)
    , p_stackDeltaIn(SgAsmInstruction::INVALID_STACK_DELTA)
    , p_semantics(nullptr) {}

// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=mnemonic         class=SgAsmInstruction
SgAsmInstruction::SgAsmInstruction(rose_addr_t const& address,
                                   std::string const& mnemonic)
    : SgAsmStatement(address)
    , p_mnemonic(mnemonic)
    , p_operandList(nullptr)
    , p_cacheLockCount(0)
    , p_stackDeltaIn(SgAsmInstruction::INVALID_STACK_DELTA)
    , p_semantics(nullptr) {}

void
SgAsmInstruction::initializeProperties() {
    p_operandList = nullptr;
    p_cacheLockCount = 0;
    p_stackDeltaIn = SgAsmInstruction::INVALID_STACK_DELTA;
    p_semantics = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
