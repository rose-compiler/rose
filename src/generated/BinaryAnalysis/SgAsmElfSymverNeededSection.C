//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverNeededSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfSymverNeededSection_IMPL
#include <sage3basic.h>

SgAsmElfSymverNeededEntryList* const&
SgAsmElfSymverNeededSection::get_entries() const {
    return p_entries;
}

void
SgAsmElfSymverNeededSection::set_entries(SgAsmElfSymverNeededEntryList* const& x) {
    changeChildPointer(this->p_entries, const_cast<SgAsmElfSymverNeededEntryList*&>(x));
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

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
