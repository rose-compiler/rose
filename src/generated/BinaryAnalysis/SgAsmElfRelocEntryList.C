//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfRelocEntryList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfRelocEntryList_IMPL
#include <sage3basic.h>

SgAsmElfRelocEntryPtrList const&
SgAsmElfRelocEntryList::get_entries() const {
    return p_entries;
}

SgAsmElfRelocEntryPtrList&
SgAsmElfRelocEntryList::get_entries() {
    return p_entries;
}

void
SgAsmElfRelocEntryList::set_entries(SgAsmElfRelocEntryPtrList const& x) {
    changeChildPointer(this->p_entries, const_cast<SgAsmElfRelocEntryPtrList&>(x));
    set_isModified(true);
}

SgAsmElfRelocEntryList::~SgAsmElfRelocEntryList() {
    destructorHelper();
}

SgAsmElfRelocEntryList::SgAsmElfRelocEntryList() {}

void
SgAsmElfRelocEntryList::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
