//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmElfSymverEntryList* const&
SgAsmElfSymverSection::get_entries() const {
    return p_entries;
}

void
SgAsmElfSymverSection::set_entries(SgAsmElfSymverEntryList* const& x) {
    this->p_entries = x;
    set_isModified(true);
}

SgAsmElfSymverSection::~SgAsmElfSymverSection() {
    destructorHelper();
}

SgAsmElfSymverSection::SgAsmElfSymverSection()
    : p_entries(createAndParent<SgAsmElfSymverEntryList>(this)) {}

void
SgAsmElfSymverSection::initializeProperties() {
    p_entries = createAndParent<SgAsmElfSymverEntryList>(this);
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
