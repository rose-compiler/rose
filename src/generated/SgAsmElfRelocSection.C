//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfRelocSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

bool const&
SgAsmElfRelocSection::get_uses_addend() const {
    return p_uses_addend;
}

void
SgAsmElfRelocSection::set_uses_addend(bool const& x) {
    this->p_uses_addend = x;
    set_isModified(true);
}

SgAsmElfSection* const&
SgAsmElfRelocSection::get_target_section() const {
    return p_target_section;
}

void
SgAsmElfRelocSection::set_target_section(SgAsmElfSection* const& x) {
    this->p_target_section = x;
    set_isModified(true);
}

SgAsmElfRelocEntryList* const&
SgAsmElfRelocSection::get_entries() const {
    return p_entries;
}

void
SgAsmElfRelocSection::set_entries(SgAsmElfRelocEntryList* const& x) {
    this->p_entries = x;
    set_isModified(true);
}

SgAsmElfRelocSection::~SgAsmElfRelocSection() {
    destructorHelper();
}

SgAsmElfRelocSection::SgAsmElfRelocSection()
    : p_uses_addend(true)
    , p_target_section(nullptr)
    , p_entries(createAndParent<SgAsmElfRelocEntryList>(this)) {}

void
SgAsmElfRelocSection::initializeProperties() {
    p_uses_addend = true;
    p_target_section = nullptr;
    p_entries = createAndParent<SgAsmElfRelocEntryList>(this);
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
