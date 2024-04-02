//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmSynthesizedFieldDeclaration            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmSynthesizedFieldDeclaration_IMPL
#include <SgAsmSynthesizedFieldDeclaration.h>

std::string const&
SgAsmSynthesizedFieldDeclaration::get_name() const {
    return p_name;
}

void
SgAsmSynthesizedFieldDeclaration::set_name(std::string const& x) {
    this->p_name = x;
    set_isModified(true);
}

uint64_t const&
SgAsmSynthesizedFieldDeclaration::get_offset() const {
    return p_offset;
}

void
SgAsmSynthesizedFieldDeclaration::set_offset(uint64_t const& x) {
    this->p_offset = x;
    set_isModified(true);
}

SgAsmSynthesizedFieldDeclaration::~SgAsmSynthesizedFieldDeclaration() {
    destructorHelper();
}

SgAsmSynthesizedFieldDeclaration::SgAsmSynthesizedFieldDeclaration()
    : p_offset(0) {}

// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
SgAsmSynthesizedFieldDeclaration::SgAsmSynthesizedFieldDeclaration(rose_addr_t const& address)
    : SgAsmSynthesizedDeclaration(address)
    , p_offset(0) {}

void
SgAsmSynthesizedFieldDeclaration::initializeProperties() {
    p_offset = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
