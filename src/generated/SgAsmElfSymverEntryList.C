//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverEntryList                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmElfSymverEntryPtrList const&
SgAsmElfSymverEntryList::get_entries() const {
    return p_entries;
}

SgAsmElfSymverEntryPtrList&
SgAsmElfSymverEntryList::get_entries() {
    return p_entries;
}

void
SgAsmElfSymverEntryList::set_entries(SgAsmElfSymverEntryPtrList const& x) {
    this->p_entries = x;
    set_isModified(true);
}

SgAsmElfSymverEntryList::~SgAsmElfSymverEntryList() {
    destructorHelper();
}

SgAsmElfSymverEntryList::SgAsmElfSymverEntryList() {}

void
SgAsmElfSymverEntryList::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
