//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSegmentTableEntryList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmElfSegmentTableEntryPtrList const&
SgAsmElfSegmentTableEntryList::get_entries() const {
    return p_entries;
}

SgAsmElfSegmentTableEntryPtrList&
SgAsmElfSegmentTableEntryList::get_entries() {
    return p_entries;
}

void
SgAsmElfSegmentTableEntryList::set_entries(SgAsmElfSegmentTableEntryPtrList const& x) {
    this->p_entries = x;
    set_isModified(true);
}

SgAsmElfSegmentTableEntryList::~SgAsmElfSegmentTableEntryList() {
    destructorHelper();
}

SgAsmElfSegmentTableEntryList::SgAsmElfSegmentTableEntryList() {}

void
SgAsmElfSegmentTableEntryList::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
