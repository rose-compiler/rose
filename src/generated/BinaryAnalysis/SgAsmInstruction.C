//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmInstruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmInstruction_IMPL
#include <SgAsmInstruction.h>

std::string const&
SgAsmInstruction::get_architectureName() const {
    return p_architectureName;
}

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
SgAsmInstruction::get_rawBytes() const {
    return p_rawBytes;
}

void
SgAsmInstruction::set_rawBytes(SgUnsignedCharList const& x) {
    this->p_rawBytes = x;
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

std::vector<SgAsmInstruction*> const&
SgAsmInstruction::get_delaySlots() const {
    return p_delaySlots;
}

std::vector<SgAsmInstruction*>&
SgAsmInstruction::get_delaySlots() {
    return p_delaySlots;
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
//    property=architectureName class=SgAsmInstruction
//    property=mnemonic         class=SgAsmInstruction
SgAsmInstruction::SgAsmInstruction(rose_addr_t const& address,
                                   std::string const& architectureName,
                                   std::string const& mnemonic)
    : SgAsmStatement(address)
    , p_architectureName(architectureName)
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
