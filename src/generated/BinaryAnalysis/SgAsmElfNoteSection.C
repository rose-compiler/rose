//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfNoteSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfNoteSection_IMPL
#include <SgAsmElfNoteSection.h>

SgAsmElfNoteEntryList* const&
SgAsmElfNoteSection::get_entries() const {
    return p_entries;
}

void
SgAsmElfNoteSection::set_entries(SgAsmElfNoteEntryList* const& x) {
    changeChildPointer(this->p_entries, const_cast<SgAsmElfNoteEntryList*&>(x));
    set_isModified(true);
}

SgAsmElfNoteSection::~SgAsmElfNoteSection() {
    destructorHelper();
}

SgAsmElfNoteSection::SgAsmElfNoteSection()
    : p_entries(createAndParent<SgAsmElfNoteEntryList>(this)) {}

void
SgAsmElfNoteSection::initializeProperties() {
    p_entries = createAndParent<SgAsmElfNoteEntryList>(this);
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
