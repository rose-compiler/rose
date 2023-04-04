//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfNoteSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmElfNoteEntryList* const&
SgAsmElfNoteSection::get_entries() const {
    return p_entries;
}

void
SgAsmElfNoteSection::set_entries(SgAsmElfNoteEntryList* const& x) {
    this->p_entries = x;
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

#endif // ROSE_ENABLE_BINARY_ANALYSIS
