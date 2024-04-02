//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfSymverSection_IMPL
#include <sage3basic.h>

SgAsmElfSymverEntryList* const&
SgAsmElfSymverSection::get_entries() const {
    return p_entries;
}

void
SgAsmElfSymverSection::set_entries(SgAsmElfSymverEntryList* const& x) {
    changeChildPointer(this->p_entries, const_cast<SgAsmElfSymverEntryList*&>(x));
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

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
