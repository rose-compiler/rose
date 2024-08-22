//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmM68kInstruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmM68kInstruction_IMPL
#include <SgAsmM68kInstruction.h>

Rose::BinaryAnalysis::M68kInstructionKind const&
SgAsmM68kInstruction::get_kind() const {
    return p_kind;
}

void
SgAsmM68kInstruction::set_kind(Rose::BinaryAnalysis::M68kInstructionKind const& x) {
    this->p_kind = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::M68kDataFormat const&
SgAsmM68kInstruction::get_dataFormat() const {
    return p_dataFormat;
}

void
SgAsmM68kInstruction::set_dataFormat(Rose::BinaryAnalysis::M68kDataFormat const& x) {
    this->p_dataFormat = x;
    set_isModified(true);
}

SgAsmM68kInstruction::~SgAsmM68kInstruction() {
    destructorHelper();
}

SgAsmM68kInstruction::SgAsmM68kInstruction()
    : p_kind(Rose::BinaryAnalysis::m68k_unknown_instruction)
    , p_dataFormat(Rose::BinaryAnalysis::m68k_fmt_unknown) {}

// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=architectureId   class=SgAsmInstruction
//    property=kind             class=SgAsmM68kInstruction
//    property=dataFormat       class=SgAsmM68kInstruction
SgAsmM68kInstruction::SgAsmM68kInstruction(rose_addr_t const& address,
                                           uint8_t const& architectureId,
                                           Rose::BinaryAnalysis::M68kInstructionKind const& kind,
                                           Rose::BinaryAnalysis::M68kDataFormat const& dataFormat)
    : SgAsmInstruction(address, architectureId)
    , p_kind(kind)
    , p_dataFormat(dataFormat) {}

void
SgAsmM68kInstruction::initializeProperties() {
    p_kind = Rose::BinaryAnalysis::m68k_unknown_instruction;
    p_dataFormat = Rose::BinaryAnalysis::m68k_fmt_unknown;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
