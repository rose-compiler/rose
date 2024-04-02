//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfNoteEntryList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfNoteEntryList_IMPL
#include <sage3basic.h>

SgAsmElfNoteEntryPtrList const&
SgAsmElfNoteEntryList::get_entries() const {
    return p_entries;
}

SgAsmElfNoteEntryPtrList&
SgAsmElfNoteEntryList::get_entries() {
    return p_entries;
}

void
SgAsmElfNoteEntryList::set_entries(SgAsmElfNoteEntryPtrList const& x) {
    this->p_entries = x;
    set_isModified(true);
}

SgAsmElfNoteEntryList::~SgAsmElfNoteEntryList() {
    destructorHelper();
}

SgAsmElfNoteEntryList::SgAsmElfNoteEntryList() {}

void
SgAsmElfNoteEntryList::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
