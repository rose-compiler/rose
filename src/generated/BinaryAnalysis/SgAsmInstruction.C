//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmInstruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmInstruction_IMPL
#include <SgAsmInstruction.h>

uint8_t const&
SgAsmInstruction::get_architectureId() const {
    return p_architectureId;
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

SgAsmInstruction* const&
SgAsmInstruction::get_delaySlot() const {
    return p_delaySlot;
}

void
SgAsmInstruction::set_delaySlot(SgAsmInstruction* const& x) {
    this->p_delaySlot = x;
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
    , p_delaySlot(nullptr)
    , p_semantics(nullptr) {}

// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=architectureId   class=SgAsmInstruction
SgAsmInstruction::SgAsmInstruction(Rose::BinaryAnalysis::Address const& address,
                                   uint8_t const& architectureId)
    : SgAsmStatement(address)
    , p_architectureId(architectureId)
    , p_operandList(nullptr)
    , p_delaySlot(nullptr)
    , p_semantics(nullptr) {}

void
SgAsmInstruction::initializeProperties() {
    p_operandList = nullptr;
    p_delaySlot = nullptr;
    p_semantics = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
