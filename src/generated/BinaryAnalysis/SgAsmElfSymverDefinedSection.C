//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverDefinedSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfSymverDefinedSection_IMPL
#include <sage3basic.h>

SgAsmElfSymverDefinedEntryList* const&
SgAsmElfSymverDefinedSection::get_entries() const {
    return p_entries;
}

void
SgAsmElfSymverDefinedSection::set_entries(SgAsmElfSymverDefinedEntryList* const& x) {
    changeChildPointer(this->p_entries, const_cast<SgAsmElfSymverDefinedEntryList*&>(x));
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

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
