//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverDefinedEntryList                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmElfSymverDefinedEntryPtrList const&
SgAsmElfSymverDefinedEntryList::get_entries() const {
    return p_entries;
}

SgAsmElfSymverDefinedEntryPtrList&
SgAsmElfSymverDefinedEntryList::get_entries() {
    return p_entries;
}

void
SgAsmElfSymverDefinedEntryList::set_entries(SgAsmElfSymverDefinedEntryPtrList const& x) {
    this->p_entries = x;
    set_isModified(true);
}

SgAsmElfSymverDefinedEntryList::~SgAsmElfSymverDefinedEntryList() {
    destructorHelper();
}

SgAsmElfSymverDefinedEntryList::SgAsmElfSymverDefinedEntryList() {}

void
SgAsmElfSymverDefinedEntryList::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
