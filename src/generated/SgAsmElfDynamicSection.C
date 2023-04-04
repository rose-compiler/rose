//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfDynamicSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmElfDynamicEntryList* const&
SgAsmElfDynamicSection::get_entries() const {
    return p_entries;
}

void
SgAsmElfDynamicSection::set_entries(SgAsmElfDynamicEntryList* const& x) {
    this->p_entries = x;
    set_isModified(true);
}

SgAsmElfDynamicSection::~SgAsmElfDynamicSection() {
    destructorHelper();
}

SgAsmElfDynamicSection::SgAsmElfDynamicSection()
    : p_entries(createAndParent<SgAsmElfDynamicEntryList>(this)) {}

void
SgAsmElfDynamicSection::initializeProperties() {
    p_entries = createAndParent<SgAsmElfDynamicEntryList>(this);
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
