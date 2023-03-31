//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPESection                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmPESectionTableEntry* const&
SgAsmPESection::get_section_entry() const {
    return p_section_entry;
}

void
SgAsmPESection::set_section_entry(SgAsmPESectionTableEntry* const& x) {
    this->p_section_entry = x;
    set_isModified(true);
}

SgAsmPESection::~SgAsmPESection() {
    destructorHelper();
}

SgAsmPESection::SgAsmPESection()
    : p_section_entry(nullptr) {}

void
SgAsmPESection::initializeProperties() {
    p_section_entry = nullptr;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
