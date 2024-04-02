//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfDynamicSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfDynamicSection_IMPL
#include <SgAsmElfDynamicSection.h>

SgAsmElfDynamicEntryList* const&
SgAsmElfDynamicSection::get_entries() const {
    return p_entries;
}

void
SgAsmElfDynamicSection::set_entries(SgAsmElfDynamicEntryList* const& x) {
    changeChildPointer(this->p_entries, const_cast<SgAsmElfDynamicEntryList*&>(x));
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

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
