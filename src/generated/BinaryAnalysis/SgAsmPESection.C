//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPESection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmPESection_IMPL
#include <SgAsmPESection.h>

SgAsmPESectionTableEntry* const&
SgAsmPESection::get_section_entry() const {
    return p_section_entry;
}

void
SgAsmPESection::set_section_entry(SgAsmPESectionTableEntry* const& x) {
    changeChildPointer(this->p_section_entry, const_cast<SgAsmPESectionTableEntry*&>(x));
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

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
