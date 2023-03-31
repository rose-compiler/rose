//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfDynamicEntryList                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmElfDynamicEntryPtrList const&
SgAsmElfDynamicEntryList::get_entries() const {
    return p_entries;
}

SgAsmElfDynamicEntryPtrList&
SgAsmElfDynamicEntryList::get_entries() {
    return p_entries;
}

void
SgAsmElfDynamicEntryList::set_entries(SgAsmElfDynamicEntryPtrList const& x) {
    this->p_entries = x;
    set_isModified(true);
}

SgAsmElfDynamicEntryList::~SgAsmElfDynamicEntryList() {
    destructorHelper();
}

SgAsmElfDynamicEntryList::SgAsmElfDynamicEntryList() {}

void
SgAsmElfDynamicEntryList::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
