//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfEHFrameEntryFDList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfEHFrameEntryFDList_IMPL
#include <SgAsmElfEHFrameEntryFDList.h>

SgAsmElfEHFrameEntryFDPtrList const&
SgAsmElfEHFrameEntryFDList::get_entries() const {
    return p_entries;
}

SgAsmElfEHFrameEntryFDPtrList&
SgAsmElfEHFrameEntryFDList::get_entries() {
    return p_entries;
}

void
SgAsmElfEHFrameEntryFDList::set_entries(SgAsmElfEHFrameEntryFDPtrList const& x) {
    this->p_entries = x;
    set_isModified(true);
}

SgAsmElfEHFrameEntryFDList::~SgAsmElfEHFrameEntryFDList() {
    destructorHelper();
}

SgAsmElfEHFrameEntryFDList::SgAsmElfEHFrameEntryFDList() {}

void
SgAsmElfEHFrameEntryFDList::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
