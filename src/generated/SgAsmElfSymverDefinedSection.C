//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverDefinedSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmElfSymverDefinedEntryList* const&
SgAsmElfSymverDefinedSection::get_entries() const {
    return p_entries;
}

void
SgAsmElfSymverDefinedSection::set_entries(SgAsmElfSymverDefinedEntryList* const& x) {
    this->p_entries = x;
    set_isModified(true);
}

SgAsmElfSymverDefinedSection::~SgAsmElfSymverDefinedSection() {
    destructorHelper();
}

SgAsmElfSymverDefinedSection::SgAsmElfSymverDefinedSection()
    : p_entries(createAndParent<SgAsmElfSymverDefinedEntryList>(this)) {}

void
SgAsmElfSymverDefinedSection::initializeProperties() {
    p_entries = createAndParent<SgAsmElfSymverDefinedEntryList>(this);
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
