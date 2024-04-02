//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSegmentTableEntryList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfSegmentTableEntryList_IMPL
#include <SgAsmElfSegmentTableEntryList.h>

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
    changeChildPointer(this->p_entries, const_cast<SgAsmElfSegmentTableEntryPtrList&>(x));
    set_isModified(true);
}

SgAsmElfSegmentTableEntryList::~SgAsmElfSegmentTableEntryList() {
    destructorHelper();
}

SgAsmElfSegmentTableEntryList::SgAsmElfSegmentTableEntryList() {}

void
SgAsmElfSegmentTableEntryList::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
