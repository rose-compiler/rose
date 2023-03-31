//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverNeededSection                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmElfSymverNeededEntryList* const&
SgAsmElfSymverNeededSection::get_entries() const {
    return p_entries;
}

void
SgAsmElfSymverNeededSection::set_entries(SgAsmElfSymverNeededEntryList* const& x) {
    this->p_entries = x;
    set_isModified(true);
}

SgAsmElfSymverNeededSection::~SgAsmElfSymverNeededSection() {
    destructorHelper();
}

SgAsmElfSymverNeededSection::SgAsmElfSymverNeededSection()
    : p_entries(createAndParent<SgAsmElfSymverNeededEntryList>(this)) {}

void
SgAsmElfSymverNeededSection::initializeProperties() {
    p_entries = createAndParent<SgAsmElfSymverNeededEntryList>(this);
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
