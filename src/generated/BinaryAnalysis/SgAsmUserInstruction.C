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
//    property=architectureName class=SgAsmInstruction
//    property=mnemonic         class=SgAsmInstruction
//    property=kind             class=SgAsmUserInstruction
SgAsmUserInstruction::SgAsmUserInstruction(rose_addr_t const& address,
                                           std::string const& architectureName,
                                           std::string const& mnemonic,
                                           unsigned const& kind)
    : SgAsmInstruction(address, architectureName, mnemonic)
    , p_kind(kind) {}

void
SgAsmUserInstruction::initializeProperties() {
    p_kind = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
