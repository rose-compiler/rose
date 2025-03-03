//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmUserInstruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmUserInstruction_IMPL
#include <SgAsmUserInstruction.h>

unsigned const&
SgAsmUserInstruction::get_kind() const {
    return p_kind;
}

void
SgAsmUserInstruction::set_kind(unsigned const& x) {
    this->p_kind = x;
    set_isModified(true);
}

SgAsmUserInstruction::~SgAsmUserInstruction() {
    destructorHelper();
}

SgAsmUserInstruction::SgAsmUserInstruction()
    : p_kind(0) {}

// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=architectureId   class=SgAsmInstruction
//    property=kind             class=SgAsmUserInstruction
SgAsmUserInstruction::SgAsmUserInstruction(Rose::BinaryAnalysis::Address const& address,
                                           uint8_t const& architectureId,
                                           unsigned const& kind)
    : SgAsmInstruction(address, architectureId)
    , p_kind(kind) {}

void
SgAsmUserInstruction::initializeProperties() {
    p_kind = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
