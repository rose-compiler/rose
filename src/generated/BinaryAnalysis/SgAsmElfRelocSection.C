//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfRelocSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfRelocSection_IMPL
#include <SgAsmElfRelocSection.h>

bool const&
SgAsmElfRelocSection::get_usesAddend() const {
    return p_usesAddend;
}

void
SgAsmElfRelocSection::set_usesAddend(bool const& x) {
    this->p_usesAddend = x;
    set_isModified(true);
}

SgAsmElfSection* const&
SgAsmElfRelocSection::get_targetSection() const {
    return p_targetSection;
}

void
SgAsmElfRelocSection::set_targetSection(SgAsmElfSection* const& x) {
    this->p_targetSection = x;
    set_isModified(true);
}

SgAsmElfRelocEntryList* const&
SgAsmElfRelocSection::get_entries() const {
    return p_entries;
}

void
SgAsmElfRelocSection::set_entries(SgAsmElfRelocEntryList* const& x) {
    changeChildPointer(this->p_entries, const_cast<SgAsmElfRelocEntryList*&>(x));
    set_isModified(true);
}

SgAsmElfRelocSection::~SgAsmElfRelocSection() {
    destructorHelper();
}

SgAsmElfRelocSection::SgAsmElfRelocSection()
    : p_usesAddend(true)
    , p_targetSection(nullptr)
    , p_entries(createAndParent<SgAsmElfRelocEntryList>(this)) {}

void
SgAsmElfRelocSection::initializeProperties() {
    p_usesAddend = true;
    p_targetSection = nullptr;
    p_entries = createAndParent<SgAsmElfRelocEntryList>(this);
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
